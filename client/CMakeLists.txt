add_custom_target(client ALL 
  COMMAND go build -o "${CMAKE_BINARY_DIR}/Output/Client/"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
