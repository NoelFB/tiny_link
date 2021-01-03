#pragma once
#include "../world.h"
#include <blah.h>

using namespace Blah;

namespace TL
{
	class Player : public Component
	{
	public:
		void update() override;
	};
}