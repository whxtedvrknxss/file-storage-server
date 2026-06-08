#pragma once

#include <filesystem>
#include <string_view>

#include <asio.hpp>

namespace fileserver::storage {

using DirectoryChildren = std::vector<std::string>;

class StorageInspector {
 public:
  virtual ~StorageInspector() = default;

  virtual bool FileExists(std::string_view path) const noexcept = 0;

  virtual DirectoryChildren ListDirectory(
      std::string_view path) const noexcept = 0;

  virtual void DeleteDir(std::string_view path) const noexcept = 0;
  virtual void DeleteFile(std::string_view path) const noexcept = 0;
};

class StorageStreamer {
 public:
  virtual ~StorageStreamer() = default;

  virtual auto OpenFileForReading(asio::any_io_executor executor,
                                  std::string_view path,
                                  std::error_code& err) const noexcept
      -> std::unique_ptr<asio::stream_file> = 0;

  virtual auto OpenFileForWriting(asio::any_io_executor executor,
                                  std::string_view path,
                                  std::error_code& err) const noexcept
      -> std::unique_ptr<asio::stream_file> = 0;

  virtual auto OpenFileForAppending(asio::any_io_executor executor,
                                    std::string_view path,
                                    std::error_code& err) const noexcept
      -> std::unique_ptr<asio::stream_file> = 0;
};

class FileSystem : public StorageInspector, public StorageStreamer {
 public:
  explicit FileSystem(std::string_view root);

 public:
  [[nodiscard]] bool FileExists(std::string_view path) const noexcept override;

  [[nodiscard]] DirectoryChildren ListDirectory(
      std::string_view path) const noexcept override;

 public:
  [[nodiscard]] auto OpenFileForReading(asio::any_io_executor executor,
                                        std::string_view path,
                                        std::error_code& err) const noexcept
      -> std::unique_ptr<asio::stream_file> override;

  [[nodiscard]] auto OpenFileForWriting(asio::any_io_executor executor,
                                        std::string_view path,
                                        std::error_code& err) const noexcept
      -> std::unique_ptr<asio::stream_file> override;

  [[nodiscard]] auto OpenFileForAppending(asio::any_io_executor executor,
                                          std::string_view path,
                                          std::error_code& err) const noexcept
      -> std::unique_ptr<asio::stream_file> override;

 private:
  const std::filesystem::path root_path_;
  const std::filesystem::path storage_path_;
};

}  // namespace fileserver::storage
