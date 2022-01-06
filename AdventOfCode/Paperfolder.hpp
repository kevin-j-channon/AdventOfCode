#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "Common.hpp"

#include "CharacterMaps.hpp"

#include <variant>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

namespace fold_direction
{
enum { x, y };
}

template<int DIRECTION>
struct Fold
{
	static constexpr int direction = DIRECTION;
	size_t value;
};

using Fold_t = std::variant<Fold<fold_direction::x>, Fold<fold_direction::y>>;

///////////////////////////////////////////////////////////////////////////////

class FoldSequence;

class Paper
{
public:

	friend class FoldSequence;

	using Point_t = Point2D<size_t>;
	using Iter_t = std::set<Point_t>::iterator;
	using Matrix_t = arma::Mat<int>;

	// These are here so that std::inserter works with Paper.
	using value_type = Point_t;	
	using iterator = Iter_t;

	Paper() {}

	Paper(const Paper&) = delete;
	Paper& operator=(const Paper&) = delete;

	Paper(Paper&&) = default;
	Paper& operator=(Paper&&) = default;

	Paper& load(std::istream& is)
	{
		auto line = std::string{};
		while (std::getline(is, line)) {
			if (line.empty()) {
				break;
			}

			mark(Point_t{}.from_string(line));
		}

		return *this;
	}

	void mark(Point_t point) { _marks.insert(std::move(point)); }
	void erase(const Point_t& point) { _marks.erase(point); }
	bool read(const Point_t& point) const { return _marks.contains(point); }

	size_t mark_count() const { return _marks.size(); }

	auto begin() const { return _marks.cbegin(); }
	auto begin() { return _marks.begin(); }
	auto end() const { return _marks.cend(); }
	auto end() { return _marks.end(); }

	auto insert(Iter_t hint, Point_t point) {
		return _marks.insert(hint, std::move(point));
	}

	Matrix_t as_matrix() const
	{
		const auto dimensions = _get_dimensions();
		const auto mat = Matrix_t(dimensions.y, dimensions.x).fill(0);
		return std::accumulate(_marks.begin(), _marks.end(), mat, [](auto matrix, auto point) {
				matrix.at(point.y, point.x) = 1;
				return std::move(matrix);
			});
	}

private:

	Point_t _get_dimensions() const {
		return {
			std::max_element(_marks.begin(), _marks.end(), [](auto m1, auto m2) { return m1.x < m2.x; })->x + 2,	// Add an extra column because the last letter is only 4 wide
			std::max_element(_marks.begin(), _marks.end(), [](auto m1, auto m2) { return m1.y < m2.y; })->y + 1
		};
	}

	std::set<Point_t> _marks;
};

///////////////////////////////////////////////////////////////////////////////

class FoldSequence : public std::ranges::view_interface<FoldSequence>
{

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

	Fold_t _load_fold(const std::string& str)
	{
		const auto fold_details = _get_fold_details_from_string(str);
		const auto value = string_to<size_t>(fold_details[1]);

		switch (fold_details[0][0] ) {
		case 'x': return Fold<fold_direction::x>{ value };
		case 'y': return Fold<fold_direction::y>{ value };
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

	std::vector<Fold_t> _folds;
};

///////////////////////////////////////////////////////////////////////////////

class PaperFolder
{
	class FolderImpl
	{
	public:

		FolderImpl(Paper p)
			: _paper{ std::move(p) }
		{}

		Paper operator()(const Fold<fold_direction::x>& fold)
		{
			auto marks_to_move = std::vector<Paper::Point_t>{};
			std::copy_if(_paper.begin(), _paper.end(), std::back_inserter(marks_to_move), [&fold](auto mark) {
				return mark.x > fold.value;
				});

			std::for_each(marks_to_move.begin(), marks_to_move.end(), [this](const auto& mark) {_paper.erase(mark); });

			std::transform(marks_to_move.begin(), marks_to_move.end(), std::inserter(_paper, _paper.end()), [&fold](auto mark) -> Paper::Point_t {
				return { 2 * fold.value - mark.x, mark.y };
				});

			return std::move(_paper);
		}

		Paper operator()(const Fold<fold_direction::y>& fold)
		{
			auto marks_to_move = std::vector<Paper::Point_t>{};
			std::copy_if(_paper.begin(), _paper.end(), std::back_inserter(marks_to_move), [&fold](auto mark) {
				return mark.y > fold.value;
				});

			std::for_each(marks_to_move.begin(), marks_to_move.end(), [this](const auto& mark) {_paper.erase(mark); });

			std::transform(marks_to_move.begin(), marks_to_move.end(), std::inserter(_paper, _paper.end()), [&fold](auto mark) -> Paper::Point_t {
				return { mark.x, 2 * fold.value - mark.y };
				});

			return std::move(_paper);
		}

	private:
		Paper _paper;
	};

public:
	PaperFolder(Paper paper)
		: _paper{ std::move(paper) }
	{}

	Paper apply(const FoldSequence& folds)
	{
		return std::accumulate(folds.begin(), folds.end(), std::move(_paper), [this](auto curr, auto fold) -> Paper {
			return apply_fold(std::move(curr), fold);
			});
	}

	static Paper apply_fold(Paper paper, const Fold_t& fold)
	{
		return std::visit(FolderImpl{ std::move(paper) }, fold);
	}

private:

	Paper _paper;
};

///////////////////////////////////////////////////////////////////////////////

template<size_t ROWS, size_t COLS>
class CharacterReader
{
public:

	using Matrix_t = arma::subview<Paper::Matrix_t::value_type>;
	
	static char decode(const Matrix_t& mat)
	{
		if (mat.n_rows != ROWS || mat.n_cols != COLS) {
			throw InvalidArgException("Character matrix is not the correct size");
		}

		const auto scores = char_maps | std::ranges::views::transform([&mat](const auto& char_map) {
			return _score_match(mat, char_map);
			});

		return _char_maps_index_to_letter(std::distance(scores.begin(), std::min_element(scores.begin(), scores.end())));
	}

private:

	static uint32_t _score_match(const Matrix_t& test, const CharMap_6x5& reference)
	{
		auto out = uint32_t{ 0 };

		for (auto r = size_t{ 0 }; r < ROWS; ++r) {
			for (auto c = 0; c < COLS; ++c) {
				out += std::abs(test.at(r, c) - reference[r][c]);
			}
		}

		return out;
	}

	static char _char_maps_index_to_letter(size_t idx)
	{
		return static_cast<char>('A' + idx);
	}
};

///////////////////////////////////////////////////////////////////////////////

template<size_t CHAR_ROWS, size_t CHAR_COLS>
class PaperReader
{
public:

	static std::string decode(const Paper::Matrix_t& paper)
	{
		const auto character_count = paper.n_cols / CHAR_COLS;

		std::stringstream out;
		
		for (auto i = 0; i < character_count; ++i) {
			const auto letter = paper.submat(0, i * CHAR_COLS, CHAR_ROWS - 1, CHAR_COLS * (i + 1) - 1);
			out << CharacterReader<CHAR_ROWS, CHAR_COLS>::decode(letter);
		}

		return out.str();
	}
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
