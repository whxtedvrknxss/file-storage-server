#include "builder.h"

namespace fileserver::http {

void RequestBuilder::OnMethod(Method method) { request_.method_ = method; }

void RequestBuilder::OnURI(std::string_view uri) { request_.uri_.append(uri); }

void RequestBuilder::OnVersion(uint8_t major, uint8_t minor) {
  if (major == 1 && (minor == 1 || minor == 0)) {
    request_.version_ = (minor == 0) ? Version::Http1_0 : Version::Http1_1;
  } else {
    request_.version_ = Version::Unsupported;
  }
}

void RequestBuilder::OnHeader(Header header) {
  request_.headers_.push_back(std::move(header));
}

// void RequestBuilder::OnBody(std::span<const char> chunk) {
//   request_.body.append_range(chunk);
// }

void RequestBuilder::OnComplete() {}

}  // namespace fileserver::http
