#pragma once
#include <inttypes.h>

namespace TL
{
	struct Mask
	{
		// bitfield masks for collision types

		static constexpr uint32_t solid = 1 << 0;
		static constexpr uint32_t jumpthru = 1 << 1;
		static constexpr uint32_t player_attack = 1 << 2;
		static constexpr uint32_t enemy = 1 << 3;
		static constexpr uint32_t player = 1 << 4;
	};
}