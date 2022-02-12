#include <blah.h>
#include "game.h"

using namespace Blah;
using namespace TL;

int main()
{
	Game game;

	Config config;
	config.name = "Sword II: Adventure of Frog";
	config.width = 1280;
	config.height = 720;

	config.on_startup  = [&]() { game.startup(); };
	config.on_shutdown = [&]() { game.shutdown(); };
	config.on_update   = [&]() { game.update(); };
	config.on_render   = [&]() { game.render(); };

	App::run(&config);
}