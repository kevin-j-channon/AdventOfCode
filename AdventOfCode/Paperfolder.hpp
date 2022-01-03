#pragma once

#include "Common.hpp"

namespace aoc
{
class Paper
{
public:

	using Point_t = Point2D<size_t>;

	Paper& load(std::istream& is)
	{
		while (!is.eof()) {
			_marks[Point_t{}.load(is)]++;
		}

		return *this;
	}

	size_t mmark_count() const { return _marks.size(); }

private:
	std::map<Point_t, size_t> _marks;
};
}
