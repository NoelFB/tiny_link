#include "game.h"
#include "content.h"
#include "masks.h"
#include "assets/tileset.h"
#include "components/collider.h"
#include "components/tilemap.h"
#include "components/player.h"
#include "factory.h"

using namespace TL;

namespace
{
	constexpr float transition_duration = 0.4f;
}

void Game::startup()
{
	// load our content
	Content::load();

	// framebuffer for the game
	buffer = FrameBuffer::create(width, height);

	// set batcher to use Nearest Filter
	batch.default_sampler = TextureSampler(TextureFilter::Nearest);

	m_draw_colliders = false;

	// camera setup
	camera = Vec2::zero;
	load_room(Point(0, 0));
}

void Game::load_room(Point cell)
{
	const Image* grid = Content::find_room(cell);
	BLAH_ASSERT(grid, "Room doesn't exist!");
	room = cell;

	// get room offset
	auto offset = Point(cell.x * width, cell.y * height);

	// get the castle tileset for now
	auto castle = Content::find_tileset("castle");
	auto grass = Content::find_tileset("grass");
	auto plants = Content::find_tileset("plants");

	// make the floor
	auto floor = world.add_entity(offset);
	auto tilemap = floor->add(Tilemap(8, 8, columns, rows));
	auto solids = floor->add(Collider::make_grid(8, 40, 23));
	solids->mask = Mask::solid;

	// loop over the room grid
	for (int x = 0; x < columns; x ++)
		for (int y = 0; y < rows; y++)
		{
			Point world_position = offset + Point(x * tile_width, y * tile_height);
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

			// castle tiles
			case 0xffffff:
				tilemap->set_cell(x, y, &castle->random_tile());
				solids->set_cell(x, y, true);
				break;

			// grass tiles
			case 0x8f974a:
				tilemap->set_cell(x, y, &grass->random_tile());
				solids->set_cell(x, y, true);
				break;

			// plants tiles
			case 0x4b692f:
				tilemap->set_cell(x, y, &plants->random_tile());
				break;

			// player (only if it doesn't already exist)
			case 0x6abe30:
				if (!world.first<Player>())
					Factory::player(&world, world_position + Point(tile_width / 2, tile_height));
				break;

			// brambles
			case 0xd77bba:
				Factory::bramble(&world, world_position + Point(tile_width / 2, tile_height));
				break;
			}
		}
}

void Game::shutdown()
{

}

void Game::update()
{
	// Toggle Collider Render
	if (Input::pressed(Key::F1))
		m_draw_colliders = !m_draw_colliders;

	// Reload Current Room
	if (Input::pressed(Key::F2))
	{
		m_transition = false;
		world.clear();
		load_room(room);
	}

	// Normal Update
	if (!m_transition)
	{
		world.update();

		auto player = world.first<Player>();
		if (player)
		{
			auto pos = player->entity()->position;
			auto bounds = RectI(room.x * width, room.y * height, width, height);
			if (!bounds.contains(pos))
			{
				// target room
				Point next_room = Point(pos.x / width, pos.y / height);
				if (pos.x < 0) next_room.x--;
				if (pos.y < 0) next_room.y--;

				// see if room exists
				if (Content::find_room(next_room))
				{
					Time::pause_for(0.1f);

					// transiton to it!
					m_transition = true;
					m_next_ease = 0;
					m_next_room = next_room;
					m_last_room = room;

					// store entities from the previous room
					m_last_entities.clear();
					Entity* e = world.first_entity();
					while (e)
					{
						m_last_entities.push_back(e);
						e = e->next();
					}

					// load contents of the next room
					load_room(next_room);
				}
				// doesn't exist, clamp player
				else
				{
					player->entity()->position = Point(
						Calc::clamp_int(pos.x, bounds.x, bounds.x + bounds.w),
						Calc::clamp_int(pos.y, bounds.y, bounds.y + bounds.h));
				}
			}
		}
	}
	// Room Transition routine
	else
	{
		// increment ease
		m_next_ease = Calc::approach(m_next_ease, 1.0f, Time::delta / transition_duration);

		// get last & next camera position
		auto last_cam = Vec2(m_last_room.x * width, m_last_room.y * height);
		auto next_cam = Vec2(m_next_room.x * width, m_next_room.y * height);

		// LERP camera position
		camera = last_cam + (next_cam - last_cam) * Ease::cube_in_out(m_next_ease);

		// Finish Transition
		if (m_next_ease >= 1.0f)
		{
			// delete old objects (except player!)
			for (auto& it : m_last_entities)
			{
				if (!it->get<Player>())
					world.destroy_entity(it);
			}

			Time::pause_for(0.1f);
			m_transition = false;
		}
	}
}

void Game::render()
{
	// draw gameplay stuff
	{
		buffer->clear(0x150e22);

		batch.push_matrix(Mat3x2::create_translation(-camera));
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

		batch.pop_matrix();
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
