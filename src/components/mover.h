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
		Collider* collider = nullptr;
		Vec2 speed;
		float gravity = 0;

		bool move_x(int amount);
		bool move_y(int amount);

		void stop_x();
		void stop_y();
		void stop();

		bool on_ground(int dist = 1) const;

		void update() override;
	};
}