#pragma once
#include "../world.h"
#include "collider.h"
#include <blah.h>
#include <functional>

using namespace Blah;

namespace TL
{
	class Hurtable : public Component
	{
	private:
		float m_cooldown_timer = 0;

	public:
		Collider* collider = nullptr;
		uint32_t hurt_by = 0;
		float cooldown = 1.0f;
		std::function<void(Hurtable* self)> on_hurt;

		void update() override;
	};
}