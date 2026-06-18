#pragma once

#include <filesystem>
#include <string_view>
#include <expected>

#include <asio.hpp>

#include "utils/defines.hpp"

namespace fileserver::storage {

using DirectoryChildren = std::vector<std::string>;
using StreamFilePtr = std::unique_ptr<asio::stream_file>;

class StorageInspector {
 public:
  virtual ~StorageInspector() = default;

  [[nodiscard]] virtual auto FileExists(std::string_view path) const noexcept
      -> utils::ExpectedErrc<bool> = 0;

  [[nodiscard]] virtual auto ListDirectory(std::string_view path) const noexcept
      -> utils::ExpectedErrc<DirectoryChildren> = 0;

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
      -> utils::ExpectedErrc<StreamFilePtr> = 0;

  [[nodiscard]] virtual auto OpenFileForWriting(
      asio::any_io_executor executor, std::string_view path) const noexcept
      -> utils::ExpectedErrc<StreamFilePtr> = 0;

  [[nodiscard]] virtual auto OpenFileForAppending(
      asio::any_io_executor executor, std::string_view path) const noexcept
      -> utils::ExpectedErrc<StreamFilePtr> = 0;
};

class FileSystem : public StorageInspector, public StorageStreamer {
 public:
  explicit FileSystem(std::string_view root);

 public:
  auto FileExists(std::string_view path) const noexcept
      -> utils::ExpectedErrc<bool> override;

  auto ListDirectory(std::string_view path) const noexcept
      -> utils::ExpectedErrc<DirectoryChildren> override;

  std::error_code DeleteDirectory(
      std::string_view path) const noexcept override;
  std::error_code DeleteFile(std::string_view path) const noexcept override;

 public:
  auto OpenFileForReading(asio::any_io_executor executor,
                          std::string_view path) const noexcept
      -> utils::ExpectedErrc<StreamFilePtr> override;

  auto OpenFileForWriting(asio::any_io_executor executor,
                          std::string_view path) const noexcept
      -> utils::ExpectedErrc<StreamFilePtr> override;

  auto OpenFileForAppending(asio::any_io_executor executor,
                            std::string_view path) const noexcept
      -> utils::ExpectedErrc<StreamFilePtr> override;

 private:
  std::filesystem::path root_path_;
  std::filesystem::path storage_path_;
};

}  // namespace fileserver::storage
