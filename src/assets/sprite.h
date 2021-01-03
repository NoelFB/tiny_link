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
		};

		String name;
		Vec2 origin;
		Vector<Animation> animations;
	};
}