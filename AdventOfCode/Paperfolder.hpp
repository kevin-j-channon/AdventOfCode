#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "Common.hpp"

#include <variant>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

class FoldSequence;

class Paper
{
public:

	friend class FoldSequence;

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

///////////////////////////////////////////////////////////////////////////////

class FoldSequence : public std::ranges::view_interface<FoldSequence>
{
	enum { x, y };

	template<int DIRECTION>
	struct FoldInfo_
	{
		static constexpr int direction = DIRECTION;
		size_t value;
	};

	using FoldInfo = std::variant<FoldInfo_<x>, FoldInfo_<y>>;

public:

	FoldSequence& load(std::istream& is)
	{
		auto line = std::string{};
		while (std::getline(is, line)) {
			if (line.empty()) {
				continue;
			}

			auto fold = _load_fold(line);
			_folds.push_back(std::move(fold));
		}

		return *this;
	}

	auto begin() const { return _folds.cbegin(); }
	auto begin() { return _folds.begin(); }
	auto end() const { return _folds.cend(); }
	auto end() { return _folds.end(); }

	auto size() const { return _folds.size(); }

private:

	FoldInfo _load_fold(const std::string& str)
	{
		const auto fold_details = _get_fold_details_from_string(str);

		switch (fold_details[0][0] ) {
		case 'x': return FoldInfo_<x>{ string_to<size_t>(fold_details[1]) };
		case 'y': return FoldInfo_<y>{ string_to<size_t>(fold_details[1]) };
		default:;
		}

		throw Exception("Invalid fold direction");
	}

	std::vector<std::string> _get_fold_details_from_string(const std::string& str)
	{
		if (str.empty()) {
			throw Exception("Failed to load fold from string");
		}

		const auto fold_details = split(split(str, ' ').back(), '=', SplitBehaviour::drop_empty);
		if (fold_details.size() != 2) {
			throw Exception("Invalid fold specification in string");
		}

		return fold_details;
	}

	std::vector<FoldInfo> _folds;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
