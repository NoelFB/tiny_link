#pragma once
#include "../world.h"
#include <blah.h>

using namespace Blah;

namespace TL
{
	class Player : public Component
	{
	public:
		VirtualStick input_move;
		VirtualButton input_jump;

		Player();
		void update() override;

	private:
		int facing = 1;
		float m_jump_timer = 0;
		bool m_on_ground;
	};
}