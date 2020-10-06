cmake_minimum_required(VERSION 2.8)
project(app_project)

include(FetchContent)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.9.1)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
add_executable(myapp
src/main.cpp 
src/tm_driver/tm_communication.cpp
src/tm_driver/tm_command.cpp 
src/tm_driver/tm_packet.cpp 
src/tm_driver/tm_driver.cpp 
src/tm_driver/tm_print.cpp
src/tm_driver/tm_robot_state.cpp
src/tm_driver/tm_sct_communication.cpp
src/tm_driver/tm_svr_communication.cpp )

target_link_libraries(myapp PRIVATE nlohmann_json::nlohmann_json)

include_directories(include src/tm_driver/tm_msgs/srv src/tm_driver/tm_msgs/msg)

install(TARGETS myapp DESTINATION bin)