#pragma once
#include <blah.h>
#include "world.h"

using namespace Blah;

namespace TL
{
	namespace Factory
	{
		Entity* player(World* world, Point position);
	}
}