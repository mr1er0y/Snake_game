cmake_minimum_required(VERSION 3.16)
project(Shake_game)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML COMPONENTS graphics window system)

add_executable(Shake_game main.cpp)

target_link_libraries(Shake_game PUBLIC ${SFML_LIBRARIES})
include_directories(${SFML_INCLUDE_DIR})
