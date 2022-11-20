# cmakelists.txt file courtesy of Mikea15 on github, https://github.com/Mikea15/SDL-Cmake-template/blob/master/CMakeLists.txt
# CMakeList.txt : CMake project for chip8Emulator, include source and define
# project specific logic here.

cmake_minimum_required (VERSION 3.7.2)

project(chip8Emulator)
set(CMAKE_CXX_STANDARD 11)

# TODO: Add tests and install targets if needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(BIN_DIR ${chip8Emulator_SOURCE_DIR}/bin)

# This path is used by FindSDL2 module. 
# It will find include and lib folders under this directory.
# This is because find command doesn't include any path for Windows.
# So it can't find anything using default paths.
# SDL2_PATH is already expected in module file. I'm using the same variable.
IF (WIN32)
  set(SDL2_PATH "C:/c++ libraries/SDL2/x86_64-w64-mingw32")
ENDIF()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(chip8Emulator 
	src/main.c 
    src/chip8.c
    src/chip8.h
)

target_link_libraries(chip8Emulator 
	${SDL2_LIBRARY}
)

install(TARGETS chip8Emulator RUNTIME DESTINATION ${BIN_DIR}/)
set_target_properties(chip8Emulator PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}/debug/
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}/release/ )

if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
	add_custom_command( TARGET chip8Emulator POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
		${SDL2_PATH}/lib/x64/SDL2.dll 
		${BIN_DIR}/debug/SDL2.dll  )
	add_custom_command( TARGET chip8Emulator POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${SDL2_PATH}/lib/x64/SDL2.dll
		${BIN_DIR}/debug/SDL2.dll  )
elseif()
	add_custom_command( TARGET chip8Emulator POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
		${SDL2_PATH}/lib/x86/SDL2.dll 
		${BIN_DIR}/debug/SDL2.dll  )
	add_custom_command( TARGET chip8Emulator POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${SDL2_PATH}/lib/x86/SDL2.dll
		${BIN_DIR}/debug/SDL2.dll  )
endif()