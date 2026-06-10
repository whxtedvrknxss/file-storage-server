#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <print>

#include "http/builder.h"
#include "http/parser.h"

namespace fileserver::testing {

using namespace std::string_literals;

TEST_CASE("HTTP request parsing", "[http_request]") {
  auto [input, expected] = GENERATE(table<std::string, http::Request>(
      {{{"GET / HTTP/1.1\r\n"
         "Host: localhost\r\n"
         "Connection: close\r\n"
         "\r\n"s},
        http::Request{
            http::Method::Get,
            http::Version::Http1_1,
            {http::Header{.name = "host", .value = "localhost"},
             http::Header{.name = "connection", .value = "close"}},
            "/",
        }},
       {{
            "GET /index.html HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "User-Agent: Mozilla/5.0\r\n"
            "Accept: text/html\r\n"
            "\r\n"s,
        },
        http::Request{
            http::Method::Get,
            http::Version::Http1_1,
            {http::Header{.name = "host", .value = "www.example.com"},
             http::Header{.name = "user-agent", .value = "mozilla/5.0"},
             http::Header{.name = "accept", .value = "text/html"}},
            "/index.html"}},

       {{"POST /users HTTP/1.1\r\n"
         "Host: example.com\r\n"
         "Content-Type: application/x-www-form-urlencoded\r\n"
         "Content-Length: 49\r\n"
         "\r\n"
         "name=FirstName+LastName&email=bsmth%40example.com\r\n"
         "\r\n"s},
        http::Request{
            http::Method::Post,
            http::Version::Http1_1,
            {http::Header{.name = "host", .value = "example.com"},
             http::Header{.name = "content-type",
                          .value = "application/x-www-form-urlencoded"},
             http::Header{.name = "content-length", .value = "49"}},
            "/users"}},
       {{"POST /submit HTTP/1.1\r\n"
         "Transfer-Encoding: chunked\r\n"
         "\r\n"
         "5\r\n"
         "hello\r\n"
         "6\r\n"
         " world\r\n"
         "0\r\n"
         "\r\n"s},
        http::Request{
            http::Method::Post,
            http::Version::Http1_1,
            {http::Header{.name = "transfer-encoding", .value = "chunked"}},
            "/submit"}},
       {{"POST /cgi-bin/process.cgi HTTP/1.1\r\n"
         "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
         "Host: www.tutorialspoint.com\r\n"
         "Content-Type: text/xml; charset=utf-8\r\n"
         "Content-Length: 20\r\n"
         "Accept-Language: en-us\r\n"
         "Accept-Encoding: gzip, deflate\r\n"
         "Connection: Keep-Alive\r\n"
         "\r\n"
         "first=Zara&last=Ali\r\n"
         "\r\n"s},
        http::Request{
            http::Method::Post,
            http::Version::Http1_1,
            {http::Header{
                 .name = "user-agent",
                 .value = "mozilla/4.0 (compatible; msie5.01; windows nt)"},
             http::Header{.name = "host", .value = "www.tutorialspoint.com"},
             http::Header{.name = "content-type",
                          .value = "text/xml; charset=utf-8"},
             http::Header{.name = "content-length", .value = "20"},
             http::Header{.name = "accept-language", .value = "en-us"},
             http::Header{.name = "accept-encoding", .value = "gzip, deflate"},
             http::Header{.name = "connection", .value = "keep-alive"}},
            "/cgi-bin/process.cgi"}}}));

  http::RequestBuilder builder;
  http::RequestParser parser(builder);

  auto result = parser.Parse(input);
  auto actual = builder.Release();

  REQUIRE(result.status == http::ParseStatus::Complete);
  REQUIRE(result.error == http::ParseError::None);

  REQUIRE(actual.GetMethod() == expected.GetMethod());
  REQUIRE(actual.GetVersion() == expected.GetVersion());
  REQUIRE(actual.GetHeaders().size() == expected.GetHeaders().size());
  REQUIRE(actual.GetUri() == expected.GetUri());
}

}  // namespace fileserver::testing
