#pragma once
#include <blah.h>
#include "../assets/sprite.h"
#include "../world.h"

using namespace Blah;

namespace TL
{
	class Animator : public Component
	{
	private:
		const Sprite* m_sprite = nullptr;
		int m_animation_index = 0;
		int m_frame_index = 0;
		float m_frame_counter = 0;

	public:
		Vec2 scale = Vec2::one;
		Point offset = Point::zero;

		Animator() = default;
		Animator(const String& sprite);

		const Sprite* sprite() const;
		const Sprite::Animation* animation() const;

		void play(const String& animation, bool restart = false);
		void update() override;
		void render(Batch& batch) override;

	private:
		bool in_valid_state() const;
	};
}