#pragma once

#include <span>

namespace fileserver::io {

class IByteSink {
 public:
  virtual ~IByteSink() = default;

  virtual void Write(std::span<const char>) = 0;
  virtual void Finish() = 0;
};

class MemorySink : IByteSink {
 public:
  MemorySink();

  void Write(std::span<const char> buffer) override;
  void Finish() override;
};

class FileSink : IByteSink {
 public:
  FileSink();

  void Write(std::span<const char> buffer) override;
  void Finish() override;
};

}  // namespace fileserver::io
