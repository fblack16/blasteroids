cmake_minimum_required(VERSION 3.25)

project(Blasteroids VERSION 0.1)

add_executable(Blasteroids blasteroids.c game_state.c)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

target_include_directories(Blasteroids PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
