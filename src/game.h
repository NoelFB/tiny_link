#pragma once
#include <blah.h>
#include "world.h"

using namespace Blah;

namespace TL
{
	class Game
	{
	public:
		static constexpr int width = 320;
		static constexpr int height = 180;
		static constexpr int tile_width = 8;
		static constexpr int tile_height = 8;

		World world;
		FrameBufferRef buffer;
		Batch batch;

		void load_map();
		void startup();
		void shutdown();
		void update();
		void render();

	private:
		bool m_draw_colliders;
	};
}