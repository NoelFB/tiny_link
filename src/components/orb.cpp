#include "orb.h"
#include "mover.h"
#include "hurtable.h"
#include "player.h"
#include "ghost_frog.h"
#include "../masks.h"
#include "../factory.h"

using namespace TL;

Point Orb::target() const
{
	auto player = world()->first<Player>();
	auto ghost = world()->first<GhostFrog>();

	if (player && ghost)
		return (towards_player ? player->entity()->position : ghost->entity()->position) + Point(0, -8);
	
	return Point(0, 0);
}

void Orb::update()
{
	auto mover = get<Mover>();
	auto diff = Vec2(target() - entity()->position).normal();
	mover->speed = diff * speed;
}

void Orb::destroyed()
{
	Factory::pop(world(), entity()->position);
}

void Orb::on_hit()
{
	towards_player = !towards_player;
	speed += 40;

	auto hurt = get<Hurtable>();
	if (towards_player)
		hurt->stun_timer = 0;
}