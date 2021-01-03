#include "game.h"
#include "content.h"
#include "masks.h"
#include "assets/tileset.h"
#include "components/collider.h"
#include "components/tilemap.h"
#include "factory.h"

using namespace TL;

void Game::startup()
{
	// load our content
	Content::load();

	// framebuffer for the game
	buffer = FrameBuffer::create(width, height);

	// set batcher to use Nearest Filter
	batch.default_sampler = TextureSampler(TextureFilter::Nearest);

	m_draw_colliders = false;

	load_room(Point(0, 0));
}

void Game::load_room(Point cell)
{
	const Image* grid = Content::find_room(cell);
	BLAH_ASSERT(grid, "Room doesn't exist!");
	room = cell;

	// destroy all the entities
	world.clear();

	// get the castle tileset for now
	auto castle = Content::find_tileset("castle");

	// make the floor
	auto floor = world.add_entity();
	auto tilemap = floor->add(Tilemap(8, 8, columns, rows));
	auto solids = floor->add(Collider::make_grid(8, 40, 23));
	solids->mask = Mask::solid;

	// loop over the room grid
	for (int x = 0; x < columns; x ++)
		for (int y = 0; y < rows; y++)
		{
			Color col = grid->pixels[x + y * columns];
			uint32_t rgb =
				((uint32_t)col.r << 16) |
				((uint32_t)col.g << 8) |
				((uint32_t)col.b);

			switch (rgb)
			{
			// black does nothing
			case 0x000000:
				break;

			// solids
			case 0xffffff:
				tilemap->set_cell(x, y, &castle->random_tile());
				solids->set_cell(x, y, true);
				break;

			// player
			case 0x6abe30:
				Factory::player(&world, Point(x * tile_width + tile_width / 2, (y + 1) * tile_height));
				break;
			}
		}
}

void Game::shutdown()
{

}

void Game::update()
{
	if (Input::pressed(Key::F1))
		m_draw_colliders = !m_draw_colliders;
	if (Input::pressed(Key::F2))
		load_room(room);

	world.update();
}

void Game::render()
{
	// draw gameplay stuff
	{
		buffer->clear(0x150e22);

		world.render(batch);

		if (m_draw_colliders)
		{
			auto collider = world.first<Collider>();
			while (collider)
			{
				collider->render(batch);
				collider = (Collider*)collider->next();
			}
		}

		batch.render(buffer);
		batch.clear();
	}

	// draw buffer to the screen
	{
		float scale = Calc::min(
			App::backbuffer->width() / (float)buffer->width(),
			App::backbuffer->height() / (float)buffer->height());

		Vec2 screen_center = Vec2(App::backbuffer->width(), App::backbuffer->height()) / 2;
		Vec2 buffer_center = Vec2(buffer->width(), buffer->height()) / 2;

		App::backbuffer->clear(Color::black);
		batch.push_matrix(Mat3x2::create_transform(screen_center, buffer_center, Vec2::one * scale, 0));
		batch.tex(buffer->attachment(0), Vec2::zero, Color::white);
		batch.pop_matrix();
		batch.render(App::backbuffer);
		batch.clear();
	}
}
