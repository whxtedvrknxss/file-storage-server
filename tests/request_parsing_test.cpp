#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <print>

#include "http1/request_builder.h"
#include "http1/parser.h"

namespace fileserver::testing {

using namespace std::string_literals;

TEST_CASE("Random HTTP/1.x requests successful parsing", "[http]") {
  auto [input, expected] = GENERATE(table<std::string, http1::Request>({
      {{"GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n"s},
       http1::Request{
           http1::Method::Get,
           http1::Version::Http1_1,
           http1::KnownHeaders{.connection = http1::ConnectionType::Close},
           "/",
       }},
      {{
           "GET /index.html HTTP/1.1\r\n"
           "Host: www.example.com\r\n"
           "User-Agent: Mozilla/5.0\r\n"
           "Accept: text/html\r\n"
           "\r\n"s,
       },
       http1::Request{http1::Method::Get, http1::Version::Http1_1,
                      http1::KnownHeaders{}, "/index.html"}},

      {{"POST /users HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 49\r\n"
        "\r\n"
        "name=FirstName+LastName&email=bsmth%40example.com\r\n"
        "\r\n"s},
       http1::Request{http1::Method::Post, http1::Version::Http1_1,
                      http1::KnownHeaders{.content_length = 49}, "/users"}},
      {{"POST /submit HTTP/1.1\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "5\r\n"
        "hello\r\n"
        "6\r\n"
        " world\r\n"
        "0\r\n"
        "\r\n"s},
       http1::Request{http1::Method::Post, http1::Version::Http1_1,
                      http1::KnownHeaders{.transfer_encoding =
                                              http1::TransferEncoding::Chunked},
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
       http1::Request{http1::Method::Post, http1::Version::Http1_1,
                      http1::KnownHeaders{
                          .connection = http1::ConnectionType::KeepAlive,
                          .content_length = 20,
                      },
                      "/cgi-bin/process.cgi"}},
  }));

  http1::RequestBuilder builder;
  http1::RequestParser parser(builder);

  auto result = parser.Parse(input);
  auto actual = builder.Release();

  REQUIRE(result.status == http1::ParseStatus::Complete);
  REQUIRE(result.error == http1::ParseError::None);

  REQUIRE(actual.GetMethod() == expected.GetMethod());
  REQUIRE(actual.GetVersion() == expected.GetVersion());
  REQUIRE(actual.GetHeaders() == expected.GetHeaders());
  REQUIRE(actual.GetUri() == expected.GetUri());
}

TEST_CASE("Invalid HTTP/1.x requests produce correct errors", "[http]") {
  auto [input, expected] = GENERATE(table<std::string, http1::ParseError>({
      {" INVALID / HTTP/1.1\r\n"
       "Header: value\r\n"
       "\r\n"s,
       http1::ParseError::InvalidMethod},
      {"HEAD / HTTP/1.1\r\n"
       "Malformed Header:value-without-space\r\n"
       "\r\n"s,
       http1::ParseError::MalformedHeader},
      {"GET /something HTTP/a.b\r\n"
       "Content-Type: memes\r\n"
       "\r\n"s,
       http1::ParseError::InvalidVersion},
      {"POST /uri< HTTP/1.1\r\n"
       "Content-Type: test\r\n"
       "\r\n"s,
       http1::ParseError::None},  // to be done
      {"GET \r\n"s,               //
       http1::ParseError::UnexpectedEof},
  }));

  http1::RequestBuilder builder;
  http1::RequestParser parser(builder);

  auto result = parser.Parse(input);

  REQUIRE(result.error == expected);
}

}  // namespace fileserver::testing
