#include "parser.h"

#include <print>
#include <ranges>

#include <llhttp.h>

namespace fileserver::http {

RequestParser::RequestParser(RequestBuilder &builder) noexcept
    : complete_{false},
      parsing_header_value_{false},
      headers_parsed_{false},
      parser_{},
      settings_{nullptr},
      builder_{&builder} {
  llhttp_settings_init(&settings_);

  settings_.on_url = OnURI;
  settings_.on_header_field = OnHeaderName;
  settings_.on_header_value = OnHeaderValue;
  settings_.on_headers_complete = OnHeadersComplete;
  // settings_.on_body = OnBody;
  settings_.on_message_complete = OnMessageComplete;

  llhttp_init(&parser_, llhttp_type_t::HTTP_REQUEST, &settings_);

  parser_.data = this;
}

ParseResult RequestParser::Parse(std::span<const char> content) {
  llhttp_errno_t err = llhttp_execute(&parser_, content.data(), content.size());
  ParseResult result = {
      .status = (err == llhttp_errno_t::HPE_OK ? ParseStatus::Complete
                                               : ParseStatus::Error),
      .error = TranslateError(err)};

  return result;
}  // namespace fileserver::http

void RequestParser::Reset() noexcept {
  llhttp_reset(&parser_);
}

ParseError RequestParser::TranslateError(llhttp_errno_t err) {
  switch (err) {
    case llhttp_errno_t::HPE_OK: {
      return ParseError::None;
    }
    case llhttp_errno_t::HPE_INVALID_METHOD: {
      return ParseError::InvalidMethod;
    }
    case llhttp_errno_t::HPE_INVALID_VERSION: {
      return ParseError::InvalidVersion;
    }
    case llhttp_errno_t::HPE_INVALID_HEADER_TOKEN: {
      return ParseError::MalformedHeader;
    }
    default: {
      return ParseError::UnexpectedEof;
    }
  }
}

// int RequestParser::OnMethod(llhttp_t *parser, const char *at, size_t length)
// {
//   auto *self = static_cast<RequestParser *>(parser->data);
//   self->builder_->OnMethod(std::string_view(at, length));
//
//   return llhttp_errno_t::HPE_OK;
// }

int RequestParser::OnURI(llhttp_t *parser, const char *at, size_t length) {
  auto *self = static_cast<RequestParser *>(parser->data);

  self->builder_->OnURI(std::string_view(at, length));
  return llhttp_errno_t::HPE_OK;
}

int RequestParser::OnHeaderName(llhttp_t *parser, const char *at,
                                size_t length) {
  auto *self = static_cast<RequestParser *>(parser->data);

  if (self->parsing_header_value_) {
    self->builder_->OnHeader(std::move(self->current_header_));
    self->current_header_ = {};
  }

  AppendRange(self->current_header_.name, std::string_view{at, length});
  return llhttp_errno_t::HPE_OK;
}

int RequestParser::OnHeaderValue(llhttp_t *parser, const char *at,
                                 size_t length) {
  auto *self = static_cast<RequestParser *>(parser->data);

  self->parsing_header_value_ = true;
  AppendRange(self->current_header_.value, std::string_view{at, length});
  return llhttp_errno_t::HPE_OK;
}

int RequestParser::OnHeadersComplete(llhttp_t *parser) {
  auto *self = static_cast<RequestParser *>(parser->data);

  if (!self->current_header_.name.empty() &&
      !self->current_header_.value.empty()) {
    self->builder_->OnHeader(std::move(self->current_header_));
    self->current_header_ = {};
  }

  self->builder_->OnVersion(parser->http_major, parser->http_minor);

  Method method = Method::Invalid;
  switch (parser->method) {
    case llhttp_method_t::HTTP_GET: {
      method = Method::Get;
      break;
    }
    case llhttp_method_t::HTTP_HEAD: {
      method = Method::Head;
      break;
    }
    case llhttp_method_t::HTTP_POST: {
      method = Method::Post;
      break;
    }
    case llhttp_method_t::HTTP_PUT: {
      method = Method::Put;
      break;
    }
    case llhttp_method_t::HTTP_DELETE: {
      method = Method::Delete;
      break;
    }
    [[unlikely]] default:
      return llhttp_errno_t::HPE_INVALID_METHOD;
  }

  self->builder_->OnMethod(method);

  return llhttp_errno_t::HPE_OK;
}

int RequestParser::OnBody(llhttp_t *parser, const char *at, size_t length) {
  auto *self = static_cast<RequestParser *>(parser->data);
  // self->builder_->OnBody(std::string_view(at, length));
  return llhttp_errno_t::HPE_OK;
}

int RequestParser::OnMessageComplete(llhttp_t *parser) {
  auto *self = static_cast<RequestParser *>(parser->data);

  self->builder_->OnComplete();

  return llhttp_errno_t::HPE_OK;
}

void RequestParser::AppendRange(std::string &destination,
                                std::string_view source) {
  auto new_size{destination.size() + source.size()};
  destination.reserve(new_size);
  destination.append_range(source | std::views::transform([](const auto c) {
                             return std::tolower(c);
                           }));
}

}  // namespace fileserver::http
