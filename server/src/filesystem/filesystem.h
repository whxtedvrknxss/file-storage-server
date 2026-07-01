#pragma once

#include <filesystem>
#include <string_view>
#include <expected>

#include <asio/stream_file.hpp>

#include "utils/defines.hpp"

namespace fileserver::storage {

#if 0

using DirectoryChildren = std::vector<std::string>;

class StorageInspector {
 public:
  virtual ~StorageInspector() = default;

  [[nodiscard]] virtual auto FileExists(std::string_view path) const noexcept
      -> utils::ErrcOr<bool> = 0;

  [[nodiscard]] virtual auto ListDirectory(std::string_view path) const
  noexcept
      -> utils::ErrcOr<DirectoryChildren> = 0;

  [[nodiscard]] virtual std::error_code DeleteDirectory(
      std::string_view path) const noexcept = 0;
  [[nodiscard]] virtual std::error_code DeleteFile(
      std::string_view path) const noexcept = 0;
};

class StorageStreamer {
 public:
  virtual ~StorageStreamer() = default;

  [[nodiscard]] virtual auto OpenFileForReading(
      asio::any_io_executor executor, std::string_view path) const noexcept
      -> utils::ErrcOr<asio::stream_file> = 0;

  [[nodiscard]] virtual auto OpenFileForWriting(
      asio::any_io_executor executor, std::string_view path) const noexcept
      -> utils::ErrcOr<asio::stream_file> = 0;

  [[nodiscard]] virtual auto OpenFileForAppending(
      asio::any_io_executor executor, std::string_view path) const noexcept
      -> utils::ErrcOr<asio::stream_file> = 0;
};

class FileSystem : public StorageInspector, public StorageStreamer {
 public:
  explicit FileSystem(std::string_view root);

 public:
  auto FileExists(std::string_view path) const noexcept
      -> utils::ErrcOr<bool> override;

  auto ListDirectory(std::string_view path) const noexcept
      -> utils::ErrcOr<DirectoryChildren> override;

  std::error_code DeleteDirectory(
      std::string_view path) const noexcept override;
  std::error_code DeleteFile(std::string_view path) const noexcept override;

 public:
  auto OpenFileForReading(asio::any_io_executor executor,
                          std::string_view path) const noexcept
      -> utils::ErrcOr<asio::stream_file> override;

  auto OpenFileForWriting(asio::any_io_executor executor,
                          std::string_view path) const noexcept
      -> utils::ErrcOr<asio::stream_file> override;

  auto OpenFileForAppending(asio::any_io_executor executor,
                            std::string_view path) const noexcept
      -> utils::ErrcOr<asio::stream_file> override;

 private:
  std::filesystem::path root_path_;
  std::filesystem::path storage_path_;
};

#endif

using DirectoryChildren = std::vector<std::string>;

class FileSystem {
 public:
  explicit FileSystem(std::string_view root);

 public:
  auto FileExists(std::string_view path) const noexcept -> utils::ErrcOr<bool>;

  auto ListDirectory(std::string_view path) const noexcept -> utils::ErrcOr<DirectoryChildren>;

  std::error_code DeleteDirectory(std::string_view path) const noexcept;
  std::error_code DeleteFile(std::string_view path) const noexcept;

 public:
  auto OpenFileForReading(asio::any_io_executor executor, std::string_view path) const noexcept
      -> utils::ErrcOr<asio::stream_file>;

  auto OpenFileForWriting(asio::any_io_executor executor, std::string_view path) const noexcept
      -> utils::ErrcOr<asio::stream_file>;

  auto OpenFileForAppending(asio::any_io_executor executor, std::string_view path) const noexcept
      -> utils::ErrcOr<asio::stream_file>;

 private:
  std::filesystem::path root_path_;
  std::filesystem::path storage_path_;
};

}  // namespace fileserver::storage
