#pragma once
#include <blah.h>

using namespace Blah;

namespace TL
{
	struct Sprite;

	class Content
	{
	public:
		static SpriteFont font;

		static FilePath path();
		static void load();
		static void unload();
		static TextureRef atlas();

		static const Sprite* find_sprite(const char* name);
	};
}