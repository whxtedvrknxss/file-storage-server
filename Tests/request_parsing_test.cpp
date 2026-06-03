#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "http/request/builder.h"
#include "http/request/parser.h"

namespace fileserver::testing {

using namespace std::string_literals;

// std::vector<std::string> requests = {

//     {"POST /users HTTP/1.1\r\n"s
//      "Host: example.com\r\n"s
//      "Content-Type: application/x-www-form-urlencoded\r\n"s
//      "Content-Length: 49\r\n"s
//      "\r\n"s
//      "name=FirstName+LastName&email=bsmth%40example.com\r\n"s
//      "\r\n"s},

//     {"GET /hello.htm HTTP/1.1\r\n"s
//      "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"s
//      "Host: www.tutorialspoint.com\r\n"s
//      "Accept-Language: en-us\r\n"s
//      "Accept-Encoding: gzip, deflate\r\n"s
//      "Connection: Keep-Alive\r\n"s
//      "\r\n"s},

//     {"POST /cgi-bin/process.cgi HTTP/1.1\r\n"s
//      "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"s
//      "Host: www.tutorialspoint.com\r\n"s
//      "Content-Type: text/xml; charset = utf - 8\r\n"s
//      "Content-Length: 60\r\n"s
//      "Accept-Language: en-us\r\n"s
//      "Accept-Encoding: gzip, deflate\r\n"s
//      "Connection: Keep-Alive\r\n"s
//      "\r\n"s
//      "first=Zara&last=Ali\r\n"s
//      "\r\n"s}};

TEST_CASE("HTTP request parsing", "[http_request]") {
  auto [input, expected] = GENERATE(table<std::string, http::Request>(
      {{{"GET / HTTP/1.1\r\n"
         "Host: localhost\r\n"
         "Connection: close\r\n"
         "\r\n"s},
        http::Request{
            .version = http::Version::Http1_1,
            .headers = {http::Header{.name = "Host", .value = "localhost"},
                        http::Header{.name = "Connection", .value = "close"}},
            .method = http::Method::Get,
            .uri = "/",
        }},
       {{
            "POST /index.html HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "User-Agent: Mozilla/5.0\r\n"
            "Accept: text/html\r\n"
            "\r\n"s,
        },
        http::Request{
            .version = http::Version::Http1_1,
            .headers =
                {http::Header{.name = "Host", .value = "www.example.com"},
                 http::Header{.name = "User-Agent", .value = "Mozilla/5.0"},
                 http::Header{.name = "Accept", .value = "text/html"}},
            .method = http::Method::Post,
            .uri = "/index.html"}}}));

  http::RequestBuilder builder;
  http::RequestParser parser(builder);

  auto result = parser.Parse(input);
  auto actual = builder.Release();

  REQUIRE(result.status == http::ParseStatus::Complete);
  REQUIRE(result.error == http::ParseError::None);

  REQUIRE(actual.method == expected.method);
  REQUIRE(actual.uri == expected.uri);
  REQUIRE(actual.version == expected.version);
  REQUIRE(actual.headers.size() == expected.headers.size());
}

}  // namespace fileserver::testing
