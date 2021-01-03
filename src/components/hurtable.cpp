#include "hurtable.h"

using namespace TL;

void Hurtable::update()
{
	if (collider && on_hurt && m_cooldown_timer <= 0)
	{
		if (collider->check(hurt_by))
		{
			m_cooldown_timer = cooldown;
			on_hurt(this);
		}
	}

	m_cooldown_timer -= Time::delta;
}
