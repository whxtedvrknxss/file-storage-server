#include "session_manager.h"

namespace fileserver::connection {

void SessionManager::Start(std::shared_ptr<Session> session) {
  sessions_.push_back(std::move(session));
}

void SessionManager::Stop(const std::shared_ptr<Session> &session) {}

void SessionManager::StopAll() {}

}  // namespace fileserver::connection
