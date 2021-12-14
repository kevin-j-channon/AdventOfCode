#pragma once

namespace aoc
{
class LanternFish
{
public:
	explicit LanternFish(uint32_t days_until_spawning)
		: _days_until_spawning{ days_until_spawning }
	{}

	uint32_t days_until_spawning() const { return _days_until_spawning; }

private:
	uint32_t _days_until_spawning;
};
}
