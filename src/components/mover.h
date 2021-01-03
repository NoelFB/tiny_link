#pragma once
#include "../world.h"
#include "collider.h"
#include <blah.h>

using namespace Blah;

namespace TL
{
	class Mover : public Component
	{
	private:
		Vec2 m_remainder;

	public:
		Collider* collider;
		Vec2 speed;

		bool move_x(int amount);
		bool move_y(int amount);

		void stop_x();
		void stop_y();
		void stop();

		void update() override;
	};
}