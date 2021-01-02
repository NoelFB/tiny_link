#pragma once
#include <blah.h>

using namespace Blah;

namespace TL
{
	class Content
	{
	public:
		static SpriteFont font;

		static FilePath path();
		static void load();
		static void unload();
	};
}