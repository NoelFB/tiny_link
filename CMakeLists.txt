cmake_minimum_required(VERSION 3.12)
project(TinyLink)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include blah
add_subdirectory(libs/blah)

# add our source
add_executable(game
	src/main.cpp
	src/world.cpp
	src/game.cpp
	src/content.cpp
	src/factory.cpp
	src/assets/sprite.cpp
	src/assets/tileset.cpp
	src/components/animator.cpp 
	src/components/collider.cpp
	src/components/player.cpp
	src/components/mover.cpp
	src/components/tilemap.cpp
	src/components/hurtable.cpp
	src/components/timer.cpp
	src/components/ghost_frog.cpp
	src/components/orb.cpp
 )

# Reference blah
target_link_libraries(game blah)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
   	set_target_properties(game PROPERTIES LINK_FLAGS "-s USE_SDL=2 -s USE_WEBGL2=1 --preload-file ${CMAKE_SOURCE_DIR}/content@/content")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()