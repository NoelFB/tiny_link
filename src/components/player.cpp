#include "player.h"
#include "mover.h"
#include "animator.h"
#include "collider.h"
#include "orb.h"
#include "../masks.h"
#include "../content.h"

using namespace TL;

namespace
{
	constexpr float max_ground_speed = 60;
	constexpr float max_air_speed = 70;
	constexpr float ground_accel = 500;
	constexpr float air_accel = 20;
	constexpr float friction = 800;
	constexpr float hurt_friction = 200;
	constexpr float gravity = 450;
	constexpr float jump_force = -105;
	constexpr float jump_time = 0.18f;
	constexpr float hurt_duration = 0.5f;
	constexpr float invincible_duration = 1.5f;
}

Player::Player()
{
	input_move = Input::register_binding(StickBinding());
	input_move->add_dpad(0);
	input_move->add_left_stick(0, 0.2f);
	input_move->add(Key::Left, Key::Right, Key::Up, Key::Down);

	input_jump = Input::register_binding(ButtonBinding());
	input_jump->press_buffer = 0.15f;
	input_jump->add(Key::X, Button::A);

	input_attack = Input::register_binding(ButtonBinding());
	input_attack->press_buffer = 0.15f;
	input_attack->add(Key::C, Button::X);
}

void Player::update()
{
	auto mover = get<Mover>();
	auto anim = get<Animator>();
	auto hitbox = get<Collider>();
	auto was_on_ground = m_on_ground;
	m_on_ground = mover->on_ground();
	int input = input_move->sign().x;

	// Sprite Stuff
	{
		// land squish
		if (!was_on_ground && m_on_ground)
			anim->scale = Vec2f(m_facing * 1.5f, 0.7f);

		// lerp scale back to one
		anim->scale = Vec2f::approach(anim->scale, Vec2f(m_facing, 1.0f), Time::delta * 4);

		// set m_facing
		anim->scale.x = Calc::abs(anim->scale.x) * m_facing;
	}

	// START
	if (m_state == st_start)
	{
		while (hitbox->check(Mask::solid))
			entity()->position.y++;

		anim->play("sword");
		m_start_timer -= Time::delta;
		if (m_start_timer <= 0)
			m_state = st_normal;
	}
	// NORMAL STATE
	else if (m_state == st_normal)
	{
		// Current Animation
		if (m_on_ground)
		{
			if (input != 0)
				anim->play("run");
			else
				anim->play("idle");
		}
		else
		{
			anim->play("jump");
		}

		// Horizontal Movement
		{
			// Acceleration
			mover->speed.x += input * (m_on_ground ? ground_accel : air_accel) * Time::delta;

			// Maxspeed
			auto maxspd = (m_on_ground ? max_ground_speed : max_air_speed);
			if (Calc::abs(mover->speed.x) > maxspd)
			{
				mover->speed.x = Calc::approach(
					mover->speed.x,
					Calc::sign(mover->speed.x) * maxspd,
					2000 * Time::delta);
			}

			// Friction
			if (input == 0 && m_on_ground)
				mover->speed.x = Calc::approach(mover->speed.x, 0, friction * Time::delta);

			// Facing Direction
			if (input != 0 && m_on_ground)
				m_facing = input;
		}

		// Invoke Jumping
		{
			if (input_jump->pressed() && mover->on_ground())
			{
				input_jump->consume_press();
				anim->scale = Vec2f(m_facing * 0.65f, 1.4f);
				mover->speed.x = input * max_air_speed;
				m_jump_timer = jump_time;
				SoundParams params;
				params.volume = 3.0f;
				Sound::play(Content::find_audio("jump.wav"), params);
			}
		}

		// Begin Attack
		if (input_attack->pressed())
		{
			input_attack->consume_press();

			m_state = st_attack;
			m_attack_timer = 0;

			if (!m_attack_collider)
				m_attack_collider = entity()->add(Collider::make_rect(Recti()));
			m_attack_collider->mask = Mask::player_attack;

			if (m_on_ground)
				mover->speed.x = 0;
		}
	}
	// ATTACK STATE
	else if (m_state == st_attack)
	{
		anim->play("attack");
		m_attack_timer += Time::delta;

		// setup hitbox
		if (m_attack_timer < 0.2f)
		{
			m_attack_collider->set_rect(Recti(-16, -12, 16, 8));
		}
		else if (m_attack_timer < 0.5f)
		{
			m_attack_collider->set_rect(Recti(8, -8, 16, 8));
		}
		else if (m_attack_collider)
		{
			m_attack_collider->destroy();
			m_attack_collider = nullptr;
		}

		// flip hitbox if you're facing left
		if (m_facing < 0 && m_attack_collider)
		{
			auto rect = m_attack_collider->get_rect();
			rect.x = -(rect.x + rect.w);
			m_attack_collider->set_rect(rect);
		}

		// end the attack
		if (m_attack_timer >= anim->animation()->duration())
		{
			anim->play("idle");
			m_state = st_normal;
		}
	}
	// HURT STATE
	else if (m_state == st_hurt)
	{
		m_hurt_timer -= Time::delta;
		if (m_hurt_timer <= 0)
			m_state = st_normal;

		mover->speed.x = Calc::approach(mover->speed.x, 0, hurt_friction * Time::delta);
	}

	// Variable Jumping
	if (m_jump_timer > 0)
	{
		mover->speed.y = -100;
		m_jump_timer -= Time::delta;
		if (!input_jump->down())
			m_jump_timer = 0;
	}

	// Invincible timer
	if (m_state != st_hurt && m_invincible_timer > 0)
	{
		if (Time::on_interval(0.05f))
			anim->visible = !anim->visible;

		m_invincible_timer -= Time::delta;
		if (m_invincible_timer <= 0)
			anim->visible = true;
	}

	// Gravity
	if (!m_on_ground)
	{
		float grav = gravity;
		if (m_state == st_normal && Calc::abs(mover->speed.y) < 20 && input_jump->down())
			grav *= 0.4f;

		mover->speed.y += grav * Time::delta;
	}

	// Hurt Check!
	if (m_invincible_timer <= 0)
	{
		auto hit = hitbox->first(Mask::enemy);
		if (hit)
		{
			Time::pause_for(0.1f);
			anim->play("hurt");

			if (m_attack_collider)
			{
				m_attack_collider->destroy();
				m_attack_collider = nullptr;
			}

			mover->speed = Vec2f(-m_facing * 100, -80);

			health--;
			m_hurt_timer = hurt_duration;
			m_invincible_timer = invincible_duration;
			m_state = st_hurt;

			// hack:
			// destroy orb
			// ideally we would have an "attack" component that the orb could
			// subscribe to, and delete itself when it hits the player. since
			// the orb currently has no way to know if it hit the player, we
			// have to add this ugly hack!
			if (hit->get<Orb>())
				hit->entity()->destroy();
		}
	}
}
