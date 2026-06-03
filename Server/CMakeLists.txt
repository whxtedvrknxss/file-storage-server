add_executable(${SERVER_EXECUTABLE_NAME})

add_library(server_config INTERFACE)
target_include_directories(server_config INTERFACE 
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(server_config INTERFACE 
  llhttpInterface
  asioInterface
  OpenSSLInterface
  jsonInterface
  spdlogInterface
)

add_library(server_connection OBJECT)
target_sources(server_connection 
  PRIVATE
    "src/connection/session_manager.cpp"
    "src/connection/session.cpp"
  PUBLIC
    "src/connection/session_manager.h"
    "src/connection/session.h"
)
target_link_libraries(server_connection PRIVATE 
  server_config
)

add_library(server_core OBJECT)
target_sources(server_core 
  PRIVATE 
    "src/core/server.cpp"
    "src/core/router.cpp"
  PUBLIC
    "src/core/server.h"
    "src/core/router.h"
)
target_link_libraries(server_core PRIVATE
  server_config
)

# add_library(server_events)

add_library(server_filesystem OBJECT)
target_sources(server_filesystem
  PRIVATE
    "src/filesystem/file_handle.cpp"
  PUBLIC
    "src/filesystem/file_handle.h"
)
target_link_libraries(server_filesystem PRIVATE 
  server_config
)

add_library(server_http OBJECT)
target_sources(server_http
  PRIVATE 
    "src/http/request/builder.cpp"
    "src/http/request/parser.cpp"
    "src/http/response_builder.cpp"
  PUBLIC
    "src/http/request/builder.h"
    "src/http/request/parser.h"
    "src/http/common.h"
    "src/http/response_builder.h"
)
target_link_libraries(server_http PRIVATE 
  server_config
)

add_library(server_utility OBJECT)
target_sources(server_utility 
  PRIVATE
    "src/utility/logger.cpp"
  PUBLIC
    "src/utility/logger.h"
)
target_link_libraries(server_utility PRIVATE 
  server_config
)

target_sources(${SERVER_EXECUTABLE_NAME} PRIVATE
  $<TARGET_OBJECTS:server_connection>
  $<TARGET_OBJECTS:server_core>
  $<TARGET_OBJECTS:server_filesystem>
  $<TARGET_OBJECTS:server_http>
  $<TARGET_OBJECTS:server_utility>
  "src/main.cpp"
)
target_link_libraries(${SERVER_EXECUTABLE_NAME} PRIVATE
  server_config
)
