#include "session_manager.h"

#include "utility/logger.h"

namespace fileserver::connection {

void SessionManager::Start(std::shared_ptr<Session> session) {
  std::scoped_lock<std::mutex> lock{mutex_};
  sessions_.push_back(std::move(session));
  sessions_.back()->Start();
}

void SessionManager::Stop(const std::shared_ptr<Session> &session) {
  std::scoped_lock<std::mutex> lock{mutex_};
  auto it = std::ranges::find(sessions_, session);
  if (it != sessions_.end()) {
    sessions_.erase(it);
  } else {
    SERVER_LOG(Error) << "Connection is not found!";
  }
}

void SessionManager::StopAll() {
  std::scoped_lock<std::mutex> lock{mutex_};
  sessions_.clear();
}

}  // namespace fileserver::connection
