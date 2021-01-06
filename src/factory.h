#pragma once
#include <blah.h>
#include "world.h"

using namespace Blah;

namespace TL
{
	// Factory to create game objects

	namespace Factory
	{
		Entity* player(World* world, Point position);
		Entity* bramble(World* world, Point position);
		Entity* pop(World* world, Point position);
		Entity* spitter(World* world, Point position);
		Entity* bullet(World* world, Point position, int direction);
		Entity* mosquito(World* world, Point position);
		Entity* door(World* world, Point position, bool wait_for_player = false);
		Entity* blob(World* world, Point position);
		Entity* ghost_frog(World* world, Point position);
		Entity* orb(World* world, Point position);
	}
}