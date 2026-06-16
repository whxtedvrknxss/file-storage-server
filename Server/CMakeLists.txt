add_executable(${SERVER_EXECUTABLE_NAME})

add_library(server_config INTERFACE)
target_include_directories(server_config INTERFACE 
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(server_config INTERFACE 
  llhttpInterface
  asioInterface
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

add_library(server_http1 OBJECT)
target_sources(server_http1
  PRIVATE 
    "src/http1/header_validator.cpp"
    "src/http1/request_builder.cpp"
    "src/http1/parser.cpp"
    "src/http1/request.cpp"
    "src/http1/router.cpp"
    "src/http1/response_factory.cpp"
  PUBLIC
    "src/http1/common.h"
    "src/http1/header_validator.h"
    "src/http1/request_builder.h"
    "src/http1/parser.h"
    "src/http1/request.h"
    "src/http1/router.h"
    "src/http1/response_factory.h"
)
target_link_libraries(server_http1 PRIVATE 
  server_config
)

add_library(server_utils OBJECT)
target_sources(server_utils
  PRIVATE
    "src/utils/logger.cpp"
  PUBLIC
    "src/utils/logger.h"
)
target_link_libraries(server_utils PRIVATE 
  server_config
)

target_sources(${SERVER_EXECUTABLE_NAME} PRIVATE
  $<TARGET_OBJECTS:server_connection>
  $<TARGET_OBJECTS:server_core>
  $<TARGET_OBJECTS:server_filesystem>
  $<TARGET_OBJECTS:server_http1>
  $<TARGET_OBJECTS:server_utility>
  "src/main.cpp"
)
target_link_libraries(${SERVER_EXECUTABLE_NAME} PRIVATE
  server_config
)
