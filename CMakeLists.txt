cmake_minimum_required(VERSION 3.0.0)
project(chip8 VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
include_directories(chip8 ${SDL2_INCLUDE_DIRS})
include_directories(chip8 ${_sdl2mixer_incdir})

add_executable(chip8 src/main.c src/chip8.h src/chip8.c)

target_link_libraries(chip8 ${SDL2_LIBRARIES})
target_link_libraries(chip8 ${_sdl2mixer_library})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
