#include "game.h"
#include "content.h"
#include "masks.h"
#include "components/collider.h"
#include "factory.h"

using namespace TL;

void Game::startup()
{
	// load our content
	Content::load();

	// framebuffer for the game
	buffer = FrameBuffer::create(320, 180);

	// set batcher to use Nearest Filter
	batch.default_sampler = TextureSampler(TextureFilter::Nearest);

	m_draw_colliders = true;

	load_map();
}

void Game::load_map()
{
	world.clear();

	// add a test player
	Factory::player(&world, Point(50, 50));

	auto floor = world.add_entity(Point(0, 100));
	auto c2 = floor->add(Collider::make_rect(RectI(0, 0, 320, 16)));
	c2->mask = Mask::solid;
}

void Game::shutdown()
{

}

void Game::update()
{
	if (Input::pressed(Key::F1))
		m_draw_colliders = !m_draw_colliders;
	if (Input::pressed(Key::F2))
		load_map();

	world.update();
}

void Game::render()
{
	// draw gameplay stuff
	{
		buffer->clear(0x4488aa);

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
