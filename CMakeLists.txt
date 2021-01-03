cmake_minimum_required(VERSION 3.6)
project(TinyLink)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include blah
add_subdirectory(libs/blah)

# add our source
add_executable(game
	src/main.cpp
	src/world.h
	src/world.cpp
	src/game.h
	src/game.cpp
	src/content.h
	src/content.cpp
	src/assets/sprite.h
	src/assets/sprite.cpp
	src/components/animator.h
	src/components/animator.cpp 
	src/components/collider.h
	src/components/collider.cpp
	src/components/player.h
	src/components/player.cpp
	src/components/mover.h
	src/components/mover.cpp
	src/masks.h
	src/factory.h
	src/factory.cpp
 "src/assets/tileset.h" "src/assets/tileset.cpp" "src/components/tilemap.h" "src/components/tilemap.cpp" "src/components/hurtable.h" "src/components/hurtable.cpp" "src/components/timer.h" "src/components/timer.cpp")

# Reference blah
target_link_libraries(game blah)

# copy SDL2 to the build directory
set(SDL2_DLL "" CACHE FILEPATH "SDL2 DLL Path")
if (SDL2_ENABLED)
	if (EXISTS ${SDL2_DLL})
		add_custom_command(
			TARGET game POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_DLL} $<TARGET_FILE_DIR:game>)
	endif()
endif()