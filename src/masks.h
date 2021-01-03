#pragma once
#include <inttypes.h>

namespace TL
{
	struct Mask
	{
		static constexpr uint32_t solid = 1 << 0;
		static constexpr uint32_t player_attack = 1 << 1;
		static constexpr uint32_t enemy = 1 << 2;
	};
}