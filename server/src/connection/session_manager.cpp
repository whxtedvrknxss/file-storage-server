#include "session_manager.h"

namespace fileserver::connection {

void SessionManager::Register(std::shared_ptr<Session> session) {
  asio::post(strand_, [this, session = std::move(session)]() mutable {
    assert(session != nullptr);
    uint64_t id = session->GetId();
    sessions_.emplace(id, std::move(session));
  });
}

void SessionManager::Unregister(std::uint64_t id) {
  asio::post(strand_, [this, id]() {
    std::size_t num = sessions_.erase(id);
    assert(num == 1);
  });
}

void SessionManager::StopAll() {
  asio::post(strand_, [this]() {
    auto local_sessions = std::move(sessions_);
    for (const auto& [id, session] : local_sessions) {
      assert(session != nullptr);
      session->Shutdown();
    }
  });
}

}  // namespace fileserver::connection
