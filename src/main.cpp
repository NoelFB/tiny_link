#include <blah.h>
#include "game.h"

using namespace Blah;
using namespace TL;

namespace
{
	Game game;

	void startup()
	{
		game.startup();
	}

	void shutdown()
	{
		game.shutdown();
	}

	void update()
	{
		game.update();
	}

	void render()
	{
		game.render();
	}
}

int main()
{
	Config config;
	config.name = "Sword II: Adventure of Frog";
	config.width = 1280;
	config.height = 720;

	config.on_startup = startup;
	config.on_shutdown = shutdown;
	config.on_update = update;
	config.on_render = render;

	App::run(&config);
}