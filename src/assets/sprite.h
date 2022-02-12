#pragma once
#include <blah.h>

using namespace Blah;

namespace TL
{
	struct Sprite
	{
		struct Frame
		{
			Subtexture image;
			float duration;
		};

		struct Animation
		{
			String name;
			Vector<Frame> frames;

			float duration() const
			{
				float d = 0;
				for (auto& it : frames)
					d += it.duration;
				return d;
			}
		};

		String name;
		Vec2f origin;
		Vector<Animation> animations;

		const Animation* get_animation(const String& name) const;
	};
}