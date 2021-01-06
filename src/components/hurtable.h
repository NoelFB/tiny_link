#pragma once
#include "../world.h"
#include "collider.h"
#include <blah.h>
#include <functional>

using namespace Blah;

namespace TL
{
	// Automatically checks if the provided collider ever overlaps
	// with something in the `hurt_by` mask. Makes it easy for enemies
	// to check if they were hit by `Mask::player_attack`
	class Hurtable : public Component
	{
	public:
		float stun_timer = 0;
		float flicker_timer = 0;
		Collider* collider = nullptr;
		uint32_t hurt_by = 0;
		std::function<void(Hurtable* self)> on_hurt;

		void hurt();
		void update() override;
	};
}