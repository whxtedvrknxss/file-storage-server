find_package(Catch2 3 REQUIRED)

add_executable(unit_tests)
target_sources(unit_tests PRIVATE 
  $<TARGET_OBJECTS:server_connection>
  $<TARGET_OBJECTS:server_core>
  $<TARGET_OBJECTS:server_filesystem>
  $<TARGET_OBJECTS:server_http1>
  $<TARGET_OBJECTS:server_utility>
  "request_parsing_test.cpp"
)

target_link_libraries(unit_tests 
  PRIVATE
    server_config
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(unit_tests)
