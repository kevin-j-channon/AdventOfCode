#pragma once

#include "Common.hpp"

namespace aoc
{

class PaperFolder;

class Paper
{
public:

	friend class PaperFolder;

	using Point_t = Point2D<size_t>;
	using Marks_t = std::map<Point_t, size_t>;

	Paper& load(std::istream& is)
	{
		auto line = std::string{};
		while (std::getline(is, line)) {
			if (line.empty()) {
				break;
			}

			_marks[Point_t{}.from_string(line)]++;
		}

		return *this;
	}

	size_t mmark_count() const { return _marks.size(); }

private:
	Marks_t _marks;
};


}
