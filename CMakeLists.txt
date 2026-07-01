cmake_minimum_required(VERSION 3.24)

if (APPLE)
  message(FATAL_ERROR "Apple/MacOS is not supported [yet].")
  
elseif (WIN32)
  # set(CMAKE_C_COMPILER clang)
  # set(CMAKE_CXX_COMPILER clang++)

elseif (UNIX)
  # set(CMAKE_C_COMPILER gcc)
  # set(CMAKE_CXX_COMPILER g++)

endif()

set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/tools/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project(FileServer LANGUAGES C CXX) 

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "\n========== CMAKE DEBUG ==========")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "=================================\n")

# get_cmake_property(_vars CACHE_VARIABLES)
# foreach(var IN LISTS _vars)
#     message(STATUS "${var} = ${${var}}")
# endforeach()

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
  ASIO_NO_DEPRECATED
  _WIN32_WINNT=0x0601
)

if (UNIX)
  target_compile_definitions(asioInterface INTERFACE
    ASIO_HAS_IO_URING
  )
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(LIBURING REQUIRED IMPORTED_TARGET liburing)
  target_link_libraries(asioInterface INTERFACE 
    PkgConfig::LIBURING
  )
endif()

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
