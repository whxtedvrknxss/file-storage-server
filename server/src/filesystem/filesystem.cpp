#include "filesystem.h"

namespace fileserver::storage {

FileSystem::FileSystem(std::string_view root)
    : root_path_{root}, storage_path_{root_path_ / "storage"} {}

auto FileSystem::FileExists(std::string_view path) const noexcept
    -> utils::ExpectedErrc<bool> {
  namespace fs = std::filesystem;
  std::error_code err;
  bool result = fs::exists(path, err);

  if (err) {
    return std::unexpected{err};
  }

  return result;
}

auto FileSystem::ListDirectory(std::string_view path) const noexcept
    -> utils::ExpectedErrc<DirectoryChildren> {
  namespace fs = std::filesystem;
  return {};
}

std::error_code FileSystem::DeleteDirectory(
    std::string_view path) const noexcept {
  return std::error_code{};
}

std::error_code FileSystem::DeleteFile(std::string_view path) const noexcept {
  return std::error_code{};
}

auto FileSystem::OpenFileForReading(asio::any_io_executor executor,
                                    std::string_view path) const noexcept
    -> utils::ExpectedErrc<StreamFilePtr> {
  std::error_code err;
  auto file = std::make_unique<asio::stream_file>(executor);
  std::string path_str{path};

  const auto flags = asio::file_base::read_only;
  file->open(path_str, flags, err);
  if (err) {
    return std::unexpected(err);
  }

  return file;
}

auto FileSystem::OpenFileForWriting(asio::any_io_executor executor,
                                    std::string_view path) const noexcept
    -> utils::ExpectedErrc<StreamFilePtr> {
  std::error_code err;
  auto file = std::make_unique<asio::stream_file>(executor);
  std::string path_str{path};

  const auto flags = asio::file_base::write_only | asio::file_base::create;
  file->open(path_str, flags, err);
  if (err) {
    return std::unexpected(err);
  }

  return file;
}

auto FileSystem::OpenFileForAppending(asio::any_io_executor executor,
                                      std::string_view path) const noexcept
    -> utils::ExpectedErrc<StreamFilePtr> {
  std::error_code err;
  auto file = std::make_unique<asio::stream_file>(executor);

  const auto flags = asio::file_base::write_only | asio::file_base::create |
                     asio::file_base::append;
  file->open(path.data(), flags, err);
  if (err) {
    return std::unexpected(err);
  }

  return file;
}

}  // namespace fileserver::storage
