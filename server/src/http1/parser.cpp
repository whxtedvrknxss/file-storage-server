#include "parser.h"

#include <print>
#include <ranges>

#include <llhttp.h>

namespace fileserver::http1 {

RequestParser::RequestParser(RequestBuilder &builder) noexcept
    : parsing_header_value_{false},
      headers_parsed_{false},
      parser_paused_{false},
      offset_{0},
      builder_{&builder} {
  llhttp_settings_init(&settings_);

  settings_.on_url = OnURI;
  settings_.on_header_field = OnHeaderName;
  settings_.on_header_value = OnHeaderValue;
  settings_.on_headers_complete = OnHeadersComplete;
  settings_.on_body = OnBody;
  settings_.on_message_complete = OnMessageComplete;

  llhttp_init(&parser_, HTTP_REQUEST, &settings_);

  parser_.data = this;
}

ParseResult RequestParser::Parse(std::span<const char> content) {
  if (parser_paused_) {
    llhttp_resume(&parser_);
    parser_paused_ = false;
  }

  const char *ptr = content.data() + offset_;
  std::size_t size = content.size() - offset_;

  llhttp_errno_t res = llhttp_execute(&parser_, ptr, size);
  ParseError err = TranslateErr(res);

  if (res == HPE_PAUSED) {
    parser_paused_ = true;
    offset_ = llhttp_get_error_pos(&parser_) - content.data();
    return {ParseStatus::HeadersComplete, err};
  }

  if (res != HPE_OK) {
    return {ParseStatus::Error, err};
  }

  if (!headers_parsed_) {
    return {ParseStatus::NeedMoreData, err};
  }

  offset_ = 0;
  return {ParseStatus::Complete, err};
}

void RequestParser::Reset() noexcept {
  llhttp_reset(&parser_);
  parsing_header_value_ = false;
  headers_parsed_ = false;
  parser_paused_ = false;
  offset_ = 0;
}

ParseError RequestParser::TranslateErr(llhttp_errno_t err) {
  switch (err) {
    case HPE_PAUSED:
      [[fallthrough]];
    case HPE_OK:
      return ParseError::None;

    case HPE_INVALID_METHOD:
      return ParseError::InvalidMethod;

    case HPE_INVALID_VERSION:
      return ParseError::InvalidVersion;

    case HPE_INVALID_HEADER_TOKEN:
      return ParseError::MalformedHeader;

    default:
      return ParseError::UnexpectedEof;
  }
}

int RequestParser::OnURI(llhttp_t *parser, const char *at, size_t length) {
  auto *self = static_cast<RequestParser *>(parser->data);

  self->builder_->OnURI(std::string_view{at, length});
  return HPE_OK;
}

int RequestParser::OnHeaderName(llhttp_t *parser, const char *at, size_t length) {
  auto *self = static_cast<RequestParser *>(parser->data);

  auto &current_header = self->current_header_;
  if (self->parsing_header_value_) {
    self->builder_->OnHeader(current_header);
    current_header = {};
  }

  AppendStringLowercase(current_header.name, std::string_view{at, length});
  return HPE_OK;
}

int RequestParser::OnHeaderValue(llhttp_t *parser, const char *at, size_t length) {
  auto *self = static_cast<RequestParser *>(parser->data);

  self->parsing_header_value_ = true;
  AppendStringLowercase(self->current_header_.value, std::string_view{at, length});
  return HPE_OK;
}

int RequestParser::OnHeadersComplete(llhttp_t *parser) {
  auto *self = static_cast<RequestParser *>(parser->data);

  auto &current_header = self->current_header_;

  bool name_empty = current_header.name.empty();
  bool value_empty = current_header.value.empty();
  if (name_empty && value_empty) {
    self->builder_->OnHeader(std::move(current_header));
    self->parsing_header_value_ = false;
    current_header = {};
  }

  auto &builder = self->builder_;

  builder->OnVersion(parser->http_major, parser->http_minor);

  Method method;
  switch (parser->method) {
    case HTTP_GET: {
      method = Method::Get;
      break;
    }
    case HTTP_HEAD: {
      method = Method::Head;
      break;
    }
    case HTTP_POST: {
      method = Method::Post;
      break;
    }
    case HTTP_PUT: {
      method = Method::Put;
      break;
    }
    case HTTP_DELETE: {
      method = Method::Delete;
    }
    case HTTP_PATCH: {
      method = Method::Patch;
      break;
    }
    default:
      method = Method::Invalid;
  }

  builder->OnMethod(method);
  self->headers_parsed_ = true;

  return HPE_PAUSED;
}

int RequestParser::OnBody(llhttp_t *parser, const char *at, size_t length) {
  auto *self = static_cast<RequestParser *>(parser->data);
  self->builder_->OnBody(std::span{at, length});
  return HPE_OK;
}

int RequestParser::OnMessageComplete(llhttp_t *parser) {
  auto *self = static_cast<RequestParser *>(parser->data);

  self->builder_->OnComplete();

  return HPE_OK;
}

void RequestParser::AppendStringLowercase(std::string &destination, std::string_view source) {
  auto new_size{destination.size() + source.size()};
  destination.reserve(new_size);

  auto view = source | std::views::transform([](const auto c) {
                return std::tolower(c);
              });

  destination.append_range(view);
}

}  // namespace fileserver::http1
