#pragma once

#include <vector>
#include <expected>
#include <system_error>

namespace fileserver::utils {

template <typename T>
using ErrcOr = std::expected<T, std::error_code>;

static constexpr std::size_t kMaxLength = 4096;
using Buffer = std::array<char, kMaxLength>;

using DynBuffer = std::vector<char>;

}  // namespace fileserver::utils
