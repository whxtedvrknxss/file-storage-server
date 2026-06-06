#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <print>

#include "http/request/builder.h"
#include "http/request/parser.h"

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
            {http::Header{.name = "Host", .value = "localhost"},
             http::Header{.name = "Connection", .value = "close"}},
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
            {http::Header{.name = "Host", .value = "www.example.com"},
             http::Header{.name = "User-Agent", .value = "Mozilla/5.0"},
             http::Header{.name = "Accept", .value = "text/html"}},
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
            {http::Header{.name = "Host", .value = "example.com"},
             http::Header{.name = "Content-Type",
                          .value = "application/x-www-form-urlencoded"},
             http::Header{.name = "Content-Length", .value = "49"}},
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
            {http::Header{.name = "Transfer-Encoding", .value = "chunked"}},
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
                 .name = "User-Agent",
                 .value = "Mozilla/4.0 (compatible; MSIE5.01; Windows NT)"},
             http::Header{.name = "Host", .value = "www.tutorialspoint.com"},
             http::Header{.name = "Content-Type",
                          .value = "text/xml; charset=utf-8"},
             http::Header{.name = "Content-Length", .value = "20"},
             http::Header{.name = "Accept-Language", .value = "en-us"},
             http::Header{.name = "Accept-Encoding", .value = "gzip, deflate"},
             http::Header{.name = "Connection", .value = "Keep-Alive"}},
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
