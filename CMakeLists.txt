cmake_minimum_required(VERSION 3.6)
project(TinyLink)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include blah
add_subdirectory(libs/blah)

# add our source
add_executable(game
	
	src/world.cpp
 )

# Reference blah
target_link_libraries(game blah)

# copy SDL2 to the build directory
set(SDL2_DLL "" CACHE FILEPATH "SDL2 DLL Path")
if (SDL2_ENABLED)
	add_custom_command(
		TARGET game POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_DLL} $<TARGET_FILE_DIR:game>)
endif()