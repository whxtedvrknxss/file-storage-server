#pragma once

#include <cstdint>

namespace fileserver::storage::units {

constexpr std::uint64_t KiB = 1024ULL;
constexpr std::uint64_t operator"" _KiB(unsigned long long value) {
  return value * KiB;
}

constexpr std::uint64_t MiB = 1024_KiB;
constexpr std::uint64_t operator"" _MiB(unsigned long long value) {
  return value * MiB;
}

constexpr std::uint64_t GiB = 1024_MiB;
constexpr std::uint64_t operator"" _GiB(unsigned long long value) {
  return value * GiB;
}

}  // namespace fileserver::storage::units
