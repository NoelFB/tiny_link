#include "content.h"
#include "assets/sprite.h"

using namespace TL;

namespace
{
	FilePath root;
	Vector<Sprite> sprites;
	Vector<Subtexture> subtextures;
	TextureRef sprite_atlas;

	struct SpriteInfo
	{
		String name;
		Aseprite aseprite;
		uint64_t pack_index;
	};
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
	Packer packer;
	packer.padding = 0;

	// load the main font
	font = SpriteFont(path() + "fonts/dogica.ttf", 8, SpriteFont::ASCII);

	// load sprites
	Vector<SpriteInfo> sprite_info;
	uint64_t pack_index = 0;
	{
		// get all the sprites
		FilePath sprite_path = path() + "sprites/";
		for (auto& it : Directory::enumerate(sprite_path, true))
		{
			if (!it.ends_with(".ase"))
				continue;

			SpriteInfo* info = sprite_info.expand();
			info->aseprite = Aseprite(it.cstr());
			info->name = String(it.cstr() + sprite_path.length(), it.end() - 4);
		}

		// add to the atlas
		for (auto& info : sprite_info)
		{
			info.pack_index = pack_index;
			for (auto& frame : info.aseprite.frames)
			{
				packer.add(pack_index, frame.image);
				pack_index++;
			}
		}
	}

	// build the atlas
	{
		packer.pack();
		sprite_atlas = Texture::create(packer.pages[0]);

		subtextures.expand(packer.entries.size());
		for (auto& entry : packer.entries)
			subtextures[entry.id] = Subtexture(sprite_atlas, entry.packed, entry.frame);
	}

	// add sprites
	for (auto& info : sprite_info)
	{
		Sprite* sprite = sprites.expand();
		sprite->name = info.name;
		sprite->origin = Vec2::zero;

		if (info.aseprite.slices.size() > 0 && info.aseprite.slices[0].has_pivot)
		{
			sprite->origin = Vec2(
				info.aseprite.slices[0].pivot.x,
				info.aseprite.slices[0].pivot.y);
		}

		for (auto& tag : info.aseprite.tags)
		{
			Sprite::Animation* anim = sprite->animations.expand();
			anim->name = tag.name;

			for (int i = tag.from; i <= tag.to; i++)
			{
				Sprite::Frame* frame = anim->frames.expand();
				frame->duration = info.aseprite.frames[i].duration / 1000.0f;
				frame->image = subtextures[info.pack_index + i];
			}
		}
	}
}

void Content::unload()
{
	font.dispose();
}

TextureRef Content::atlas()
{
	return sprite_atlas;
}

const Sprite* Content::find_sprite(const char* name)
{
	for (auto& it : sprites)
		if (it.name == name)
			return &it;

	return nullptr;
}
