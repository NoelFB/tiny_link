#pragma once
#include "../world.h"
#include <blah.h>

using namespace Blah;

namespace TL
{
	class Hurtable;
	class Mover;
	class Collider;

	class GhostFrog : public Component
	{
	public:
		static constexpr int st_waiting = 0;
		static constexpr int st_readying_attack = 1;
		static constexpr int st_perform_slash = 2;
		static constexpr int st_floating = 3;
		static constexpr int st_shoot = 4;
		static constexpr int st_reflect = 5;
		static constexpr int st_dead_state = 6;

		static constexpr int max_health_1 = 10;
		static constexpr int max_health_2 = 3;
		int health = max_health_1;
		int phase = 0;

		GhostFrog();

		void awake() override;
		void update() override;
		void on_hurt(Hurtable* hurtable);
		void on_hit_x(Mover* mover);
		void on_hit_y(Mover* mover);

	private:
		void set_state(int state);
		float m_timer = 0;
		int m_state = st_waiting;
		int m_facing = 1;
		int m_side = 1;
		int m_reflect_count = 0;
		Point m_home;
		Point m_last_pos;
		Collider* m_attack_collider = nullptr;
	};
}