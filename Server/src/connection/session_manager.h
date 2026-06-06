#pragma once

#include "session.h"

namespace fileserver::connection {

class SessionManager {
 public:

  SessionManager() = default;

  void Start(std::shared_ptr<Session> session);
  void Stop(const std::shared_ptr<Session> &session);

  void StopAll();

 private:
  std::vector<std::shared_ptr<Session>> sessions_;
};

}  // namespace fileserver::connection
