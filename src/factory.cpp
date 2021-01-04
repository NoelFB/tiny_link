#include "factory.h"
#include "masks.h"
#include "components/animator.h"
#include "components/collider.h"
#include "components/mover.h"
#include "components/player.h"
#include "components/hurtable.h"
#include "components/timer.h"

using namespace TL;

Entity* Factory::player(World* world, Point position)
{
	auto en = world->add_entity(position);

	auto anim = en->add(Animator("player"));
	anim->play("idle");
	anim->depth = -10;

	auto hitbox = en->add(Collider::make_rect(RectI(-4, -8, 8, 8)));

	auto mover = en->add(Mover());
	mover->collider = hitbox;

	en->add(Player());
	return en;
}

Entity* Factory::bramble(World* world, Point position)
{
	auto en = world->add_entity(position);

	auto anim = en->add(Animator("bramble"));
	anim->play("idle");
	anim->depth = -5;

	auto hitbox = en->add(Collider::make_rect(RectI(-4, -8, 8, 8)));
	hitbox->mask = Mask::enemy;

	auto hurtable = en->add(Hurtable());
	hurtable->hurt_by = Mask::player_attack;
	hurtable->collider = hitbox;
	hurtable->on_hurt = [](Hurtable* self)
	{
		Time::pause_for(0.1f);
		pop(self->world(), self->entity()->position + Point(0, -4));
		self->entity()->destroy();
	};

	return en;
}

Entity* Factory::pop(World* world, Point position)
{
	auto en = world->add_entity(position);

	auto anim = en->add(Animator("pop"));
	anim->play("pop");
	anim->depth = -20;

	auto timer = en->add(Timer(anim->animation()->duration(), [](Timer* self)
	{
		self->entity()->destroy();
	}));

	return en;
}

Entity* Factory::spitter(World* world, Point position)
{
	auto en = world->add_entity(position);

	auto anim = en->add(Animator("spitter"));
	anim->play("idle");
	anim->depth = -5;

	auto hitbox = en->add(Collider::make_rect(RectI(-6, -12, 12, 12)));
	hitbox->mask = Mask::enemy;

	auto hurtable = en->add(Hurtable());
	hurtable->hurt_by = Mask::player_attack;
	hurtable->collider = hitbox;
	hurtable->on_hurt = [](Hurtable* self) mutable
	{
		Time::pause_for(0.1f);
		pop(self->world(), self->entity()->position + Point(0, -4));
		self->entity()->destroy();
	};

	auto timer = en->add(Timer(1.0f, [](Timer* self)
	{
		bullet(self->world(), self->entity()->position + Point(-8, -8), -1);
		self->get<Animator>()->play("shoot");
		self->entity()->add(Timer(0.4f, [](Timer* self) { self->get<Animator>()->play("idle"); }));
		self->start(3.0f);
	}));

	return en;
}

Entity* Factory::bullet(World* world, Point position, int direction)
{
	auto en = world->add_entity(position);

	auto anim = en->add(Animator("bullet"));
	anim->play("idle");
	anim->depth = -5;

	auto hitbox = en->add(Collider::make_rect(RectI(-4, -4, 8, 8)));
	hitbox->mask = Mask::enemy;

	auto mover = en->add(Mover());
	mover->collider = hitbox;
	mover->speed = Vec2(direction * 40, 0);
	mover->gravity = 130;
	mover->on_hit_x = [](Mover* self) { self->entity()->destroy(); };
	mover->on_hit_y = [](Mover* self) { self->speed.y = -60; };

	auto hurtable = en->add(Hurtable());
	hurtable->hurt_by = Mask::player_attack;
	hurtable->collider = hitbox;
	hurtable->on_hurt = [](Hurtable* self) mutable
	{
		Time::pause_for(0.1f);
		pop(self->world(), self->entity()->position + Point(0, -4));
		self->entity()->destroy();
	};

	en->add(Timer(2.5f, [](Timer* self)
	{
		self->get<Hurtable>()->flicker_timer = 100;
	}));

	en->add(Timer(3.0f, [](Timer* self)
	{
		self->entity()->destroy();
	}));

	return en;
}

namespace
{
	class MosquitoBehavior : public Component
	{
	public:
		int health = 2;
		float timer = 0;

		void update() override
		{
			auto mover = get<Mover>();
			auto player = world()->first<Player>();
			if (player)
			{
				auto diff = player->entity()->position.x - entity()->position.x;
				auto dist = Calc::abs(diff);

				if (dist < 100)
					mover->speed.x += Calc::sign(diff) * 100 * Time::delta;
				else
					mover->speed.x = Calc::approach(mover->speed.x, 0, 100 * Time::delta);

				if (Calc::abs(mover->speed.x) > 50)
					mover->speed.x = Calc::approach(mover->speed.x, Calc::sign(mover->speed.x) * 50, 800 * Time::delta);

				mover->speed.y = Calc::sin(timer) * 10;
			}

			timer += Time::delta * 4;
		}

		void hurt()
		{
			health--;
			if (health <= 0)
			{
				Factory::pop(world(), entity()->position);
				entity()->destroy();
			}
			else
			{
				auto mover = get<Mover>();
				auto player = world()->first<Player>();
				auto sign = Calc::sign(player->entity()->position.x - entity()->position.x);
				mover->speed.x = -sign * 140;
			}
		}
	};
}

Entity* Factory::mosquito(World* world, Point position)
{
	auto en = world->add_entity(position);
	auto mosquito = en->add(MosquitoBehavior());

	auto anim = en->add(Animator("mosquito"));
	anim->play("fly");
	anim->depth = -5;

	auto hitbox = en->add(Collider::make_rect(RectI(-4, -4, 8, 8)));
	hitbox->mask = Mask::enemy;

	auto mover = en->add(Mover());

	auto hurtable = en->add(Hurtable());
	hurtable->hurt_by = Mask::player_attack;
	hurtable->collider = hitbox;
	hurtable->on_hurt = [](Hurtable* self) { self->get<MosquitoBehavior>()->hurt(); };

	return en;
}

