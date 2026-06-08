#include "filesystem.h"

namespace fileserver::storage {

FileSystem::FileSystem(std::string_view root)
    : root_path_{root}, storage_path_{root_path_ / "storage"} {}

[[nodiscard]] bool FileSystem::FileExists(
    std::string_view path) const noexcept {
  namespace fs = std::filesystem;
  auto file_path = storage_path_ / path;
  return fs::exists(file_path);
}

[[nodiscard]] auto FileSystem::ListDirectory(
    std::string_view path) const noexcept -> std::vector<std::string> {
  namespace fs = std::filesystem;
  auto dir_path = storage_path_ / path;
  std::vector<std::string> files;
  for (const auto& dir_entry : fs::directory_iterator{dir_path}) {
    if (dir_entry.is_directory()) {
      files.push_back(dir_entry.path().filename().string() + '/');
    } else {
      files.push_back(dir_entry.path().filename().string());
    }
  }
  return files;
}

[[nodiscard]] auto FileSystem::OpenFileForReading(
    asio::any_io_executor executor, std::string_view path,
    std::error_code& err) const noexcept -> std::unique_ptr<asio::stream_file> {
  auto file = std::make_unique<asio::stream_file>(executor);
  std::string path_str{path};

  const auto flags = asio::file_base::read_only;
  if (file->open(path_str, flags, err)) {
    return nullptr;
  }

  return file;
}

[[nodiscard]] auto FileSystem::OpenFileForWriting(
    asio::any_io_executor executor, std::string_view path,
    std::error_code& err) const noexcept -> std::unique_ptr<asio::stream_file> {
  auto file = std::make_unique<asio::stream_file>(executor);
  std::string path_str{path};

  const auto flags = asio::file_base::write_only | asio::file_base::create;
  if (file->open(path_str, flags, err)) {
    return nullptr;
  }

  return file;
}

[[nodiscard]] auto FileSystem::OpenFileForAppending(
    asio::any_io_executor executor, std::string_view path,
    std::error_code& err) const noexcept -> std::unique_ptr<asio::stream_file> {
  auto file = std::make_unique<asio::stream_file>(executor);
  std::string path_str{path};

  const auto flags = asio::file_base::write_only | asio::file_base::create |
                     asio::file_base::append;
  if (file->open(path_str, flags, err)) {
    return nullptr;
  }
  return file;
}

}  // namespace fileserver::storage
