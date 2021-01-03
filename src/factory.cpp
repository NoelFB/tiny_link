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

