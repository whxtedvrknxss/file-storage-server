#pragma once

#include <span>

#include <llhttp.h>

#include "request_builder.h"

namespace fileserver::http1 {

enum class ParseStatus : std::uint8_t {
  NeedMoreData,
  HeadersComplete,
  Complete,
  Error,
};

enum class ParseError : std::uint8_t {
  None,
  InvalidMethod,
  InvalidURI,
  InvalidVersion,
  MalformedHeader,
  InvalidContentLength,
  UnexpectedEof,
  BodyTooLarge,
};

struct ParseResult {
  ParseStatus status;
  ParseError error;
};

class RequestParser {
 public:
  explicit RequestParser(RequestBuilder &builder) noexcept;

  RequestParser() = delete;
  RequestParser(RequestParser &&) = delete;
  RequestParser(const RequestParser &) = delete;
  RequestParser &operator=(RequestParser &&) = delete;
  RequestParser &operator=(const RequestParser &) = delete;
  ~RequestParser() = default;

  [[nodiscard]] ParseResult Parse(std::span<const char> content);

  void Reset() noexcept;

 private:
  static ParseError TranslateErr(llhttp_errno_t err);

 private:
  static int OnURI(llhttp_t *, const char *, size_t);
  static int OnHeaderName(llhttp_t *, const char *, size_t);
  static int OnHeaderValue(llhttp_t *, const char *, size_t);
  static int OnHeadersComplete(llhttp_t *);
  static int OnBody(llhttp_t *, const char *, size_t);
  static int OnMessageComplete(llhttp_t *);

 private:
  static void AppendStringLowercase(std::string &destination, std::string_view source);

 private:
  bool parsing_header_value_;
  bool headers_parsed_;
  bool parser_paused_;
  std::uintptr_t offset_;
  Header current_header_;

  llhttp_t parser_;
  llhttp_settings_t settings_;

  RequestBuilder *builder_;
};

}  // namespace fileserver::http1
