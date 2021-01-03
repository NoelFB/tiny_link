#include "animator.h"
#include "../content.h"

using namespace TL;

Animator::Animator(const String& sprite)
{
	m_sprite = Content::find_sprite(sprite);
	m_animation_index = 0;
}

const Sprite* Animator::sprite() const
{
	return m_sprite;
}

const Sprite::Animation* Animator::animation() const
{
	if (m_sprite && m_animation_index >= 0 && m_animation_index < m_sprite->animations.size())
		return &m_sprite->animations[m_animation_index];
	return nullptr;
}

void Animator::play(const String& animation, bool restart)
{
	BLAH_ASSERT(m_sprite, "No Sprite Assigned!");

	for (int i = 0; i < m_sprite->animations.size(); i++)
	{
		if (m_sprite->animations[i].name == animation)
		{
			if (m_animation_index != i || restart)
			{
				m_animation_index = i;
				m_frame_index = 0;
				m_frame_counter = 0;
			}

			break;
		}
	}
}

void Animator::update()
{
	// only update if we're in a valid state
	if (in_valid_state())
	{
		// quick references
		auto& anim = m_sprite->animations[m_animation_index];
		auto& frame = anim.frames[m_frame_index];

		// increment frame counter
		m_frame_counter += Time::delta;

		// move to next frame after duration
		while (m_frame_counter >= frame.duration)
		{
			// reset frame counter
			m_frame_counter -= frame.duration;

			// increement frame, move back if we're at the end
			m_frame_index++;
			if (m_frame_index >= anim.frames.size())
				m_frame_index = 0;
		}
	}
}

void Animator::render(Batch& batch)
{
	if (in_valid_state())
	{
		batch.push_matrix(
			Mat3x2::create_transform(entity()->position, m_sprite->origin, scale, 0));

		auto& anim = m_sprite->animations[m_animation_index];
		auto& frame = anim.frames[m_frame_index];
		batch.tex(frame.image, Vec2::zero, Color::white);

		batch.pop_matrix();
	}
}

bool Animator::in_valid_state() const
{
	return 
		m_sprite &&
		m_animation_index >= 0 &&
		m_animation_index < m_sprite->animations.size() &&
		m_frame_index >= 0 &&
		m_frame_index < m_sprite->animations[m_animation_index].frames.size();
}
