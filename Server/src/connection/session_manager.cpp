#include "session_manager.h"

namespace fileserver::connection {

void SessionManager::Register(std::shared_ptr<Session> session) {
  assert(session != nullptr);

  asio::post(strand_, [this, session = std::move(session)]() mutable {
    uint64_t id = session->GetId();
    sessions_.insert({id, std::move(session)});
  });
}

void SessionManager::Unregister(const std::shared_ptr<Session>& session) {
  assert(session != nullptr);

  asio::post(strand_, [this, session]() {
    uint64_t id = session->GetId();
    sessions_.erase(id);
  });
}

void SessionManager::StopAll() {
  asio::post(strand_, [this]() {
    auto local_sessions = std::move(sessions_);

    sessions_.clear();

    for (const auto& [id, session] : local_sessions) {
      assert(session != nullptr);
      session->Shutdown();
    }
  });
}

}  // namespace fileserver::connection
