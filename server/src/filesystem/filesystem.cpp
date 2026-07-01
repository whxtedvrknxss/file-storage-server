#include "filesystem.h"

#include <asio/stream_file.hpp>

namespace fileserver::storage {

FileSystem::FileSystem(std::string_view root)
    : root_path_{root}, storage_path_{root_path_ / "storage"} {
}

auto FileSystem::FileExists(std::string_view path) const noexcept -> utils::ErrcOr<bool> {
  namespace fs = std::filesystem;
  std::error_code err;
  bool result = fs::exists(path, err);

  if (err) {
    return std::unexpected{err};
  }

  return result;
}

auto FileSystem::ListDirectory(std::string_view path) const noexcept
    -> utils::ErrcOr<DirectoryChildren> {
  namespace fs = std::filesystem;
  const fs::path directory{root_path_ / path};
  DirectoryChildren result{};

  for (const auto &entry : fs::directory_iterator{directory}) {
    std::string child = entry.path().generic_string();

    result.push_back(std::move(child));
  }

  return result;
}

std::error_code FileSystem::DeleteDirectory(std::string_view path) const noexcept {
  namespace fs = std::filesystem;
  std::error_code err;
  auto qty = fs::remove_all(path, err);

  return err;
}

std::error_code FileSystem::DeleteFile(std::string_view path) const noexcept {
  namespace fs = std::filesystem;
  std::error_code err;
  std::ignore = fs::remove(path, err);

  return err;
}

auto FileSystem::OpenFileForReading(asio::any_io_executor executor,
                                    std::string_view path) const noexcept
    -> utils::ErrcOr<asio::stream_file> {
  std::error_code err;
  asio::stream_file file{executor};
  std::string path_str{path};

  const auto flags = asio::file_base::read_only;
  file.open(path_str, flags, err);
  if (err) {
    return std::unexpected(err);
  }

  return file;
}

auto FileSystem::OpenFileForWriting(asio::any_io_executor executor,
                                    std::string_view path) const noexcept
    -> utils::ErrcOr<asio::stream_file> {
  std::error_code err;
  asio::stream_file file{executor};
  std::string path_str{path};

  const auto flags = asio::file_base::write_only | asio::file_base::create;
  file.open(path_str, flags, err);
  if (err) {
    return std::unexpected(err);
  }

  return file;
}

auto FileSystem::OpenFileForAppending(asio::any_io_executor executor,
                                      std::string_view path) const noexcept
    -> utils::ErrcOr<asio::stream_file> {
  std::error_code err;
  asio::stream_file file{executor};

  const auto flags =
      asio::file_base::write_only | asio::file_base::create | asio::file_base::append;
  file.open(path.data(), flags, err);
  if (err) {
    return std::unexpected(err);
  }

  return file;
}

}  // namespace fileserver::storage
