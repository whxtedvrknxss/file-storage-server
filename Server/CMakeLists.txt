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
target_compile_options(server_config INTERFACE 
  $<$<CXX_COMPILER_ID:MSVC>:/EHs-c->
  $<$<CXX_COMPILER_ID:Clang>:-fno-exceptions>
  $<$<CXX_COMPILER_ID:GNU>:-fno-exceptions>
) 
target_compile_definitions(server_config INTERFACE 
  $<$<CXX_COMPILER_ID:MSVC>:_HAS_EXCEPTIONS=0>
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
  PUBLIC
    "src/core/server.h"
)
target_link_libraries(server_core PRIVATE
  server_config
)

# add_library(server_events)

add_library(server_filesystem OBJECT)
target_sources(server_filesystem
  PRIVATE
    "src/filesystem/filesystem.cpp"
  PUBLIC
    "src/filesystem/filesystem.h"
    "src/filesystem/sizes.h"
)
target_link_libraries(server_filesystem PRIVATE 
  server_config
)

add_library(server_http OBJECT)
target_sources(server_http
  PRIVATE 
    "src/http/builder.cpp"
    "src/http/parser.cpp"
    "src/http/request.cpp"
    "src/http/router.cpp"
    "src/http/response_builder.cpp"
  PUBLIC
    "src/http/builder.h"
    "src/http/common.h"
    "src/http/parser.h"
    "src/http/request.h"
    "src/http/router.h"
    "src/http/response_builder.h"
)
target_link_libraries(server_http PRIVATE 
  server_config
)

add_library(server_utility OBJECT)
target_sources(server_utility 
  PRIVATE
    "src/utils/logger.cpp"
  PUBLIC
    "src/utils/logger.h"
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
