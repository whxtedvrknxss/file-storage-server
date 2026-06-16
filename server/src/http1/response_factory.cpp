#include "response_factory.h"

namespace fileserver::http1 {

void ResponseFactory::Build(StatusCode status) {
  response_bytes_.append_range(std::format("{} {}\r\n",
                                           VersionToString(response_.version),
                                           StatusToString(response_.status)));
  for (const auto &[name, value] : response_.headers) {
    response_bytes_.append_range(std::format("{}: {}\r\n", name, value));
  }

  // TODO: change response body formatting(bodies can get very large)
  response_bytes_.append_range(std::format("\r\n{}\r\n\r\n", response_.body));
}

void ResponseFactory::Reset() {
  response_ = {};
}

ResponseFactory &ResponseFactory::AddStatusCode(StatusCode status) {
  response_.status = status;
  return *this;
}

ResponseFactory &ResponseFactory::AddVersion(Version version) {
  response_.version = version;
  return *this;
}

ResponseFactory &ResponseFactory::AddHeader(Header header) {
  response_.headers.push_back(std::move(header));
  return *this;
}

std::string ResponseFactory::StatusToString(StatusCode status) {
  switch (status) {
    case StatusCode::Ok: {
      return "200 Ok";
    }
    case StatusCode::Created: {
      return "201 Created";
    }
    case StatusCode::BadRequest: {
      return "400 Bad Request";
    }
    case StatusCode::NotFound: {
      return "404 Not Found";
    }
    case StatusCode::ContentTooLarge: {
      return "413 Content Too Large";
    }
    case StatusCode::URITooLong: {
      return "414 URI Too Long";
    }
    case StatusCode::IAmATeapot: {
      return "418 I'm a teapot";
    }

    default:
    case StatusCode::InternalServerError: {
      return "500 Internal Server Error";
    }

    case StatusCode::NotImplemented: {
      return "501 Not Implemented";
    }
    case StatusCode::HTTPVersionNotSupported: {
      return "505 HTTP Version Not Supported";
    }
  }
}

std::string ResponseFactory::VersionToString(Version version) {
  switch (version) {
    case Version::Http1_0: {
      return "HTTP/1.0";
    }
    default:
    case Version::Http1_1: {
      return "HTTP/1.1";
    }
  }
}

}  // namespace fileserver::http1
