#pragma once
#include "../world.h"
#include <blah.h>

using namespace Blah;

namespace TL
{
	class Player : public Component
	{
	public:
		static constexpr int st_normal = 0;
		static constexpr int st_attack = 1;

		VirtualStick input_move;
		VirtualButton input_jump;
		VirtualButton input_attack;

		Player();
		void update() override;

	private:
		int m_state = st_normal;
		int m_facing = 1;
		float m_jump_timer = 0;
		float m_attack_timer = 0;
		bool m_on_ground;
	};
}