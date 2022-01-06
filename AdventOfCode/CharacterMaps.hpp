#pragma once

namespace aoc
{

template<size_t ROWS, size_t COLS>
using CharMap = std::array< std::array<int, COLS>, ROWS>;

using CharMap_6x5 = CharMap<6, 5>;

constexpr std::array<CharMap_6x5, 26> char_maps = {
	std::array{
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0}
	},
	std::array{
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 1, 0}
	},
	std::array{
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0}
	},
	std::array{
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 0, 0}
	},
	std::array{
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 1, 1, 1, 0}
	},
	std::array{
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0}
	},
	std::array{
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 1, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0}
	},
	std::array{
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0}
	},
	std::array{
		std::array{0, 1, 1, 1, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 1, 1, 1, 0}
	},
	std::array{
		std::array{0, 0, 1, 1, 0},
		std::array{0, 0, 0, 1, 0},
		std::array{0, 0, 0, 1, 0},
		std::array{0, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0}
	},
	std::array{
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 1, 0, 0},
		std::array{1, 1, 0, 0, 0},
		std::array{1, 1, 0, 0, 0},
		std::array{1, 0, 1, 0, 0},
		std::array{1, 0, 0, 1, 0}
	}, 
	std::array{
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 1, 1, 1, 0}
	},
	std::array{
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0}
	},
	std::array{
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 0, 1, 0},
		std::array{1, 0, 1, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0}
	},
	std::array{
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0}
	},
	std::array{
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0}
	},
	std::array{
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 1, 1, 0},
		std::array{0, 1, 1, 1, 0}
	},
	std::array{
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 1, 0, 0},
		std::array{1, 0, 0, 1, 0}
	},
	std::array{
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{0, 1, 1, 0, 0},
		std::array{0, 0, 0, 1, 0},
		std::array{1, 1, 1, 0, 0}
	},
	std::array{
		std::array{1, 1, 1, 1, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0}
	},
	std::array{
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0}
	},
	std::array{
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 1, 0, 0},
		std::array{1, 1, 0, 0, 0},
		std::array{1, 0, 0, 0, 0}
	},
	std::array{
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 1, 0}
	},
	std::array{
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0},
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0}
	},
	std::array{
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0}
	},
	std::array{
		std::array{1, 1, 1, 1, 0},
		std::array{0, 0, 0, 1, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 1, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 1, 1, 1, 0}
	}
};

}
