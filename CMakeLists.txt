cmake_minimum_required (VERSION 3.7.2)

project(chip8)
set(CMAKE_CXX_STANDARD 11)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_MIXER_INCLUDE_DIR})

add_executable(chip8 
	src/main.c 
    src/chip8.c
    src/chip8.h
)

target_link_libraries(chip8 ${SDL2_LIBRARIES})
target_link_libraries(chip8 ${SDL2_MIXER_LIBRARIES})