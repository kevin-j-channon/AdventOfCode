#pragma once

namespace aoc
{

namespace character
{
enum
{
	A, B, C, D, E, F, G, H, I, J, K, L, M,
	N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};
}

template<size_t ROWS, size_t COLS>
using CharMap = std::array< std::array<int, COLS>, ROWS>;

std::map<CharMap<6, 5>, char> char_maps = {
	{ {
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0}
	}, 'A'},
	{ {
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 1, 0}
	}, 'B'},
	{ {
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0}
	}, 'C'},
	{ {
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 0, 0}
	}, 'D'},
	{ {
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 1, 1, 1, 0}
	}, 'E'},
	{ {
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0}
	}, 'F'},
	{ {
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 1, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0}
	}, 'G'},
	{ {
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0}
	}, 'H'},
	{ {
		std::array{0, 1, 1, 1, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 1, 1, 1, 0}
	}, 'I'},
	{ {
		std::array{0, 0, 1, 1, 0},
		std::array{0, 0, 0, 1, 0},
		std::array{0, 0, 0, 1, 0},
		std::array{0, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0}
	}, 'J'},
	{ {
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 1, 0, 0},
		std::array{1, 1, 0, 0, 0},
		std::array{1, 1, 0, 0, 0},
		std::array{1, 0, 1, 0, 0},
		std::array{1, 0, 0, 1, 0}
	}, 'K'},
	{ {
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 1, 1, 1, 0}
	}, 'L'},
	{ {
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0}
	}, 'M'},
	{ {
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 0, 1, 0},
		std::array{1, 0, 1, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0}
	}, 'N'},
	{ {
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0}
	}, 'O'},
	{ {
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 0, 0, 0, 0}
	}, 'P'},
	{ {
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 1, 1, 0},
		std::array{0, 1, 1, 1, 0}
	}, 'Q'},
	{ {
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 0, 0},
		std::array{1, 0, 1, 0, 0},
		std::array{1, 0, 0, 1, 0}
	}, 'R'},
	{ {
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{0, 1, 1, 0, 0},
		std::array{0, 0, 0, 1, 0},
		std::array{1, 1, 1, 0, 0}
	}, 'S'},
	{ {
		std::array{1, 1, 1, 1, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0}
	}, 'T'},
	{ {
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0}
	}, 'U'},
	{ {
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 1, 0, 0},
		std::array{1, 1, 0, 0, 0},
		std::array{1, 0, 0, 0, 0}
	}, 'V'},
	{ {
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 1, 1, 1, 0},
		std::array{1, 0, 0, 1, 0}
	}, 'W'},
	{ {
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0},
		std::array{0, 1, 1, 0, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0}
	}, 'X'},
	{ {
		std::array{1, 0, 0, 1, 0},
		std::array{1, 0, 0, 1, 0},
		std::array{0, 1, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 0, 1, 0, 0}
	}, 'Y'},
	{ {
		std::array{1, 1, 1, 1, 0},
		std::array{0, 0, 0, 1, 0},
		std::array{0, 0, 1, 0, 0},
		std::array{0, 1, 0, 0, 0},
		std::array{1, 0, 0, 0, 0},
		std::array{1, 1, 1, 1, 0}
	}, 'Z' }
};

}
