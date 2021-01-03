#include "player.h"
#include "mover.h"

using namespace TL;

namespace
{
	constexpr float max_ground_speed = 60;
	constexpr float max_air_speed = 70;
	constexpr float ground_accel = 500;
	constexpr float air_accel = 20;
	constexpr float friction = 800;
	constexpr float gravity = 450;
	constexpr float jump_force = -105;
	constexpr float jump_time = 0.18f;
}

Player::Player()
{
	input_move = VirtualStick()
		.add_keys(Key::Left, Key::Right, Key::Up, Key::Down)
		.add_buttons(0, Button::Left, Button::Right, Button::Up, Button::Down)
		.add_axes(0, Axis::LeftX, Axis::LeftY, 0.2f);

	input_jump = VirtualButton()
		.press_buffer(0.15f)
		.add_key(Key::X)
		.add_button(0, Button::A);
}

void Player::update()
{
	input_move.update();
	input_jump.update();

	auto mover = get<Mover>();
	auto on_ground = mover->on_ground();
	int input = input_move.value_i().x;

	// Horizontal Movement
	{

		// Acceleration
		mover->speed.x += input * (on_ground ? ground_accel : air_accel) * Time::delta;

		// Maxspeed
		auto maxspd = (on_ground ? max_ground_speed : max_air_speed);
		if (Calc::abs(mover->speed.x) > maxspd)
		{
			mover->speed.x = Calc::approach(
				mover->speed.x, 
				Calc::sign(mover->speed.x) * maxspd,
				2000 * Time::delta);
		}

		if (input == 0 && on_ground)
			mover->speed.x = Calc::approach(mover->speed.x, 0, friction * Time::delta);
	}

	// Gravity
	if (!on_ground)
	{
		float grav = gravity;
		if (Calc::abs(mover->speed.y) < 20 && input_jump.down())
			grav *= 0.4f;

		mover->speed.y += grav * Time::delta;
	}

	// Jumping
	{
		if (input_jump.pressed() && mover->on_ground())
		{
			mover->speed.x = input * max_air_speed;
			m_jump_timer = jump_time;
		}

		if (m_jump_timer > 0)
		{
			mover->speed.y = -100;
			m_jump_timer -= Time::delta;
			if (!input_jump.down())
				m_jump_timer = 0;
		}
	}
}
