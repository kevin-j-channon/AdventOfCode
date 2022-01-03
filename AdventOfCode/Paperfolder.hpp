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
			auto m = Point_t{};
			is >> m;
			_marks[m]++;
		}

		return *this;
	}

	size_t mmark_count() const { return _marks.size(); }

private:
	std::map<Point_t, size_t> _marks;
};
}
