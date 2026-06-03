add_executable(${CLIENT_EXECUTABLE_NAME})

target_sources(${CLIENT_EXECUTABLE_NAME} PRIVATE 
  "src/main.cpp"
)

target_link_libraries(${CLIENT_EXECUTABLE_NAME} PRIVATE 
  asioInterface
  OpenSSLInterface
  jsonInterface
  spdlogInterface
)

