#include "sprite.h"

using namespace TL;

const Sprite::Animation* TL::Sprite::get_animation(const String& name) const
{
	for (auto& it : animations)
		if (it.name == name)
			return &it;
	return nullptr;
}
