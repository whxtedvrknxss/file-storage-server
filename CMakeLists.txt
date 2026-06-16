cmake_minimum_required(VERSION 3.24)
project(FileServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SERVER_EXECUTABLE_NAME server)
set(CLIENT_EXECUTABLE_NAME client)

add_subdirectory(server)
add_subdirectory(client)

find_package(asio REQUIRED)
add_library(asioInterface INTERFACE)
target_include_directories(asioInterface INTERFACE 
  ${ASIO_INCLUDE_DIR}
)
target_compile_definitions(asioInterface INTERFACE 
  ASIO_STANDALONE
  _WIN32_WINNT=0x0601
)

find_package(nlohmann_json REQUIRED)
add_library(jsonInterface INTERFACE)
target_link_libraries(jsonInterface INTERFACE 
  nlohmann_json::nlohmann_json
)

find_package(llhttp REQUIRED)
add_library(llhttpInterface INTERFACE)
target_link_libraries(llhttpInterface INTERFACE
  llhttp::llhttp_static
)

find_package(spdlog REQUIRED)
add_library(spdlogInterface INTERFACE)
target_link_libraries(spdlogInterface INTERFACE 
  spdlog::spdlog
)

set_target_properties(${SERVER_EXECUTABLE_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/server")
set_target_properties(${CLIENT_EXECUTABLE_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/client"
)

enable_testing()
option(BUILD_TESTING "Build the test suite" ON)
if (BUILD_TESTING)
  add_subdirectory(tests)
  set_target_properties(unit_tests PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/unit_tests"
  )
endif()
