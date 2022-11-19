#pragma once
#include <blah.h>

using namespace Blah;

namespace TL
{
	struct Sprite;
	struct Tileset;

	class Content
	{
	public:
		static SpriteFont font;

		static FilePath path();
		static void load();
		static void unload();
		static TextureRef atlas();

		static const Sprite* find_sprite(const char* name);
		static const Tileset* find_tileset(const char* name);
		static const Image* find_room(const Point& cell);
		static const AudioRef find_audio(const char* name);
	};
}