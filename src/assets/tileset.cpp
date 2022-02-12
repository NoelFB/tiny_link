#include "tileset.h"
#include "../game.h"

using namespace TL;

const Subtexture& Tileset::random_tile() const
{
	int i = Game::rand_int(0, columns * rows);
	return tiles[i];
}
