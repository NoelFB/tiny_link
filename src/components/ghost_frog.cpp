#include "ghost_frog.h"
#include "mover.h"
#include "hurtable.h"
#include "player.h"
#include "animator.h"
#include "orb.h"
#include "../masks.h"
#include "../factory.h"
#include "../game.h"

using namespace TL;

GhostFrog::GhostFrog()
{

}

void GhostFrog::awake()
{
	m_home = entity()->position;
}

void GhostFrog::update()
{
	m_timer += Time::delta;

	auto player = world()->first<Player>();
	auto mover = get<Mover>();
	auto anim = get<Animator>();
	auto hitbox = get<Collider>();

	// no player - turn off AI
	if (!player)
		return;

	auto x = entity()->position.x;
	auto y = entity()->position.y;
	auto player_x = player->entity()->position.x;

	// update sprite
	if (mover->on_ground())
	{

	}

	// flip sprite
	anim->scale = Vec2f(m_facing, 1);

	// NORMAL STATE
	if (m_state == st_readying_attack)
	{
		m_facing = Calc::sign(player_x - x);
		if (m_facing == 0)
			m_facing = 1;

		float target_x = player_x + 32 * -m_facing;
		mover->speed.x = Calc::approach(mover->speed.x, Calc::sign(target_x - x) * 40, 400 * Time::delta);
		mover->friction = 100;
		anim->play("run");

		if (m_timer > 3.0f ||
			(m_timer > 1.0f && hitbox->check(Mask::solid, Point(-m_facing * 8, 0))))
		{
			mover->speed.x = 0;
			set_state(st_perform_slash);
		}
	}
	// SLASH STATE
	else if (m_state == st_perform_slash)
	{
		// start attack anim
		anim->play("attack");
		mover->friction = 500;

		// after 0.8s, do the lunge
		if (Time::on_time(m_timer, 0.8f))
		{
			mover->speed.x = m_facing * 250;
			hitbox->set_rect(Recti(-4 + m_facing * 4, -12, 8, 12));

			Recti rect(8, -8, 20, 8);
			if (m_facing < 0)
				rect.x = -(rect.x + rect.w);

			if (m_attack_collider)
				m_attack_collider->destroy();
			m_attack_collider = entity()->add(Collider::make_rect(rect));
			m_attack_collider->mask = Mask::enemy;
		}
		// turn off attack collider
		else if (Time::on_time(m_timer, anim->animation()->duration() - 1.0f))
		{
			if (m_attack_collider)
				m_attack_collider->destroy();
			m_attack_collider = nullptr;
		}
		// end attack state
		else if (m_timer >= anim->animation()->duration())
		{
			hitbox->set_rect(Recti(-4, -12, 8, 12));

			if (health > 0)
			{
				set_state(st_readying_attack);
			}
			else
			{
				phase = 1;
				health = max_health_2;
				m_side = Game::rand_int(0, 2) == 0 ? -1 : 1;
				set_state(st_floating);
			}
		}
	}
	// FLOATING STATE
	else if (m_state == st_floating)
	{
		anim->play("float");

		mover->friction = 0;
		mover->collider = nullptr;

		float target_y = m_home.y - 50;
		float target_x = m_home.x + m_side * 50;

		if (Calc::sign(target_y - y) != Calc::sign(target_y - m_last_pos.y))
		{
			mover->speed.y = 0;
			entity()->position.y = target_y;
		}
		else
			mover->speed.y = Calc::approach(mover->speed.y, Calc::sign(target_y - y) * 50, 800 * Time::delta);

		if (Calc::abs(y - target_y) < 8)
			mover->speed.x = Calc::approach(mover->speed.x, Calc::sign(target_x - x) * 80, 800 * Time::delta);
		else
			mover->speed.x = 0;

		if (m_timer > 5.0f || (Calc::abs(target_x - x) < 8 && Calc::abs(target_y - y) < 8))
			set_state(st_shoot);
	}
	// SHOOTING STATE
	else if (m_state == st_shoot)
	{
		mover->speed = Vec2f::approach(mover->speed, Vec2f::zero, 300 * Time::delta);

		m_facing = Calc::sign(player_x - x);
		if (m_facing == 0)
			m_facing = 1;

		if (Time::on_time(m_timer, 1.0f))
		{
			anim->play("reflect");
		}
		else if (Time::on_time(m_timer, 1.2f))
		{
			Factory::orb(world(), entity()->position + Point(m_facing * 12, -8));
			m_reflect_count = 0;
		}
		else if (Time::on_time(m_timer, 1.4f))
		{
			anim->play("float");
			set_state(st_reflect);
		}
	}
	// REFLECT STATE
	else if (m_state == st_reflect)
	{
		if (Time::on_time(m_timer, 0.4f))
			anim->play("float");

		auto orb = world()->first<Orb>();
		if (!orb)
		{
			if (m_timer > 1.0f)
			{
				m_side = -m_side;
				set_state(st_floating);
			}
		}
		else if (!orb->towards_player)
		{
			if (m_reflect_count < 2)
			{
				if (Vec2f(orb->entity()->position - orb->target()).length() < 16)
				{
					auto sign = Calc::sign(orb->entity()->position.x - x);
					if (sign != 0)
						m_facing = sign;

					anim->play("reflect");
					orb->on_hit();

					m_reflect_count++;
					m_timer = 0;
				}
			}
			else
			{
				if (Vec2f(orb->entity()->position - orb->target()).length() < 8)
				{
					Factory::pop(world(), entity()->position + Point(0, -8));
					orb->entity()->destroy();
					get<Hurtable>()->hurt();
					m_timer = 0;
				}
			}
		}
	}
	// DEAD STATE
	else if (m_state == st_dead_state)
	{
		anim->play("dead");
		world()->game->shake(1.0f);

		if (Time::on_interval(0.25f))
		{
			auto offset = Point(Game::rand_int(-16, 16), Game::rand_int(-16, 16));
			Factory::pop(world(), entity()->position + Point(0, -8) + offset);
		}

		if (Time::on_time(m_timer, 3.0f))
		{
			for (int x = -1; x < 2; x ++)
				for (int y = -1; y < 2; y ++)
					Factory::pop(world(), entity()->position + Point(x * 12, -8 + y * 12));

			Time::pause_for(0.3f);
			world()->game->shake(0.1f);
			entity()->destroy();
		}
	}

	if (m_state == st_floating || m_state == st_shoot || m_state == st_reflect)
	{
		anim->offset.y = Calc::sin(Time::seconds * 2) * 3;
	}

	m_last_pos = entity()->position;
}

void GhostFrog::on_hurt(Hurtable* hurtable)
{
	if (health > 0)
	{
		health--;

		if (health <= 0 && phase > 0)
		{
			set_state(st_dead_state);
		}

		if (m_state == st_waiting)
		{
			Factory::pop(world(), entity()->position + Point(0, -8));
			Time::pause_for(0.25f);
			set_state(st_readying_attack);
		}
	}
}

void GhostFrog::on_hit_x(Mover* mover)
{

}

void GhostFrog::on_hit_y(Mover* mover)
{

}

void GhostFrog::set_state(int state)
{
	m_state = state;
	m_timer = 0;
}
