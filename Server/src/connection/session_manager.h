#pragma once

#include <unordered_map>

#include "session.h"

namespace fileserver::connection {

class SessionManager {
 public:
  SessionManager(const asio::any_io_executor &executor) : strand_{executor} {}

  SessionManager(const SessionManager &other) = delete;
  SessionManager(SessionManager &&other) = delete;
  SessionManager &operator=(const SessionManager &other) = delete;
  SessionManager &operator=(SessionManager &&other) = delete;
  ~SessionManager() = default;

  void Register(std::shared_ptr<Session> session);
  void Unregister(const std::shared_ptr<Session> &session);

  void StopAll();

 private:
  std::unordered_map<uint64_t, std::shared_ptr<Session>> sessions_;
  asio::strand<asio::any_io_executor> strand_;
};

}  // namespace fileserver::connection
