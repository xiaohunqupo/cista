#pragma once

#include <filesystem>

#include "cista/memory_holder.h"
#include "cista/serialization.h"

namespace cista {

template <mode Mode = mode::WITH_STATIC_VERSION | mode::WITH_INTEGRITY,
          typename T>
void write(std::filesystem::path const& p, T const& w) {
  auto mmap =
      cista::mmap{p.generic_string().c_str(), cista::mmap::protection::WRITE};
  auto writer = cista::buf<cista::mmap>(std::move(mmap));
  cista::serialize<Mode>(writer, w);
}

template <mode Mode = mode::WITH_STATIC_VERSION | mode::WITH_INTEGRITY,
          typename T>
void write(std::filesystem::path const& p, wrapped<T> const& w) {
  write<Mode>(p, *w);
}

template <typename T,
          mode Mode = mode::WITH_STATIC_VERSION | mode::WITH_INTEGRITY>
cista::wrapped<T> read(std::filesystem::path const& p) {
  auto b = cista::file{p.generic_string().c_str(), "r"}.content();
  auto const ptr = cista::deserialize<T, Mode>(b);
  auto mem = cista::memory_holder{std::move(b)};
  return cista::wrapped{std::move(mem), ptr};
}

template <typename T,
          mode Mode = mode::WITH_STATIC_VERSION | mode::WITH_INTEGRITY>
cista::wrapped<T> read_mmap(std::filesystem::path const& p) {
  auto mmap =
      cista::mmap{p.generic_string().c_str(), cista::mmap::protection::READ};
  auto const ptr = cista::deserialize<T, Mode>(mmap);
  auto mem = cista::memory_holder{buf{std::move(mmap)}};
  return cista::wrapped{std::move(mem), ptr};
}

}  // namespace cista