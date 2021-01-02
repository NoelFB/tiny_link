#include "content.h"

using namespace TL;

namespace
{
	FilePath root;
}

SpriteFont Content::font;

FilePath Content::path()
{
	if (root.length() <= 0)
	{
		FilePath up = "";
		do
		{
			root = Path::normalize(FilePath::fmt("%s/%scontent/", App::path(), up.cstr()));
			up.append("../");
		} while (!Directory::exists(root) && up.length() < 30);

		if (!Directory::exists(root))
			BLAH_ERROR("Unable to find content directory!");

		Log::print("Content Path: %s", root.cstr());
	}

	return root;
}

void Content::load()
{
	font = SpriteFont(path() + "fonts/dogica.ttf", 8, SpriteFont::ASCII);
}

void Content::unload()
{
	font.dispose();
}
