#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "Common.hpp"
#include "DiagnosticLog.hpp"

#include <algorithm>
#include <vector>
#include <format>
#include <istream>
#include <map>
#include <bitset>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

using Direction = Vec2d<int>;

///////////////////////////////////////////////////////////////////////////////

struct Aiming
{
	int x{};
	int aim{};
	int depth{};

	Direction to_direction() const
	{
		return { x, depth };
	}

	Aiming operator+(const Direction& d) const
	{
		const auto new_aim = this->aim + d.y;
		return {
			this->x + d.x,
			new_aim,
			this->depth + d.x * new_aim
		};
	}
};

///////////////////////////////////////////////////////////////////////////////

class LifeSupport
{
public:
	LifeSupport(const DiagnosticLog& log) : _log(log) {}

	static uint32_t score_entry(const DiagnosticLog::Entry_t& entry, const DiagnosticLog::Entry_t& target)
	{
		return static_cast<uint32_t>(std::distance(entry.begin(), std::mismatch(entry.begin(), entry.end(), target.begin()).first));
	}

	template<typename BitSelector_T>
	uint32_t filter_bits_matching(BitSelector_T bit_selector) const
	{
		auto entries = std::vector<DiagnosticLog::Entry_t>(_log.begin(), _log.end());

		auto curr_bit_idx = uint32_t{ 0 };
		auto end = entries.end();
		while (std::distance(entries.begin(), end) > 1) {
			auto bits_to_match = bit_selector(entries.begin(), end);
			end = std::remove_if(entries.begin(), end, [&bits_to_match, curr_bit_idx](auto e) {
				return e[curr_bit_idx] != bits_to_match[curr_bit_idx];
				});

			++curr_bit_idx;
		}

		return DiagnosticLog::entry_as<uint32_t>(entries.front());
	}

	uint32_t filter_using_most_frequent_bits() const
	{
		return filter_bits_matching(DiagnosticLog::most_frequent_bits<DiagnosticLog::Iterator_t>);
	}

	uint32_t filter_using_least_frequent_bits() const
	{
		return filter_bits_matching(DiagnosticLog::least_frequent_bits<DiagnosticLog::Iterator_t>);
	}

	uint32_t rating() const
	{
		const auto o2_gen_rating = filter_using_most_frequent_bits();
		const auto co2_scrubber_rating = filter_using_least_frequent_bits();

		return o2_gen_rating * co2_scrubber_rating;
	}

private:
	const DiagnosticLog& _log;
};

///////////////////////////////////////////////////////////////////////////////

struct LogProcessor
{
	static uint32_t power_consumption(const DiagnosticLog& log)
	{
		const auto most_frequent_bits = log.get_most_frequent_bits();
		return DiagnosticLog::entry_as<uint32_t>(most_frequent_bits) * DiagnosticLog::flipped_entry_as<uint32_t>(most_frequent_bits);
	}

	static uint32_t life_support_rating(const DiagnosticLog& log)
	{
		return LifeSupport(log).rating();
	}
};

///////////////////////////////////////////////////////////////////////////////

class VentAnalyzer
{
	using Line_t = Line2d<uint32_t>;
	using Point_t = Vec2d<Line_t::Value_t>;
public:
	enum Formation
	{
		horizontal = Line_t::horizontal,
	    vertical   = Line_t::vertical,
		diagonal   = Line_t::diagonal,
	};

	VentAnalyzer(std::istream& data_stream)
		: _data_stream{ data_stream }
	{}

	template<size_t FORMATIONS>
	uint32_t score() const
	{
		auto lines = _load_lines(_data_stream);
		auto relevant_lines = _filter_for<FORMATIONS>(std::move(lines));
		auto point_densities = _calculate_point_densities<FORMATIONS>(std::move(relevant_lines));

		return _calculate_score(std::move(point_densities));
	}

private:

	static std::vector<Line_t> _load_lines(std::istream& is)
	{
		auto line = Line_t{};
		auto lines = std::vector<Line_t>{};
		while (!is.eof() && !is.fail()){
			is >> line;
			lines.push_back(line);
		}

		return lines;
	}

	template<size_t FORMATIONS>
	static std::vector<Line_t> _filter_for(std::vector<Line_t> lines)
	{
		auto should_be_removed = [](auto&& line) -> bool {
			if constexpr (static_cast<bool>(FORMATIONS & Formation::horizontal)) {
				if (is_horizontal(line)) {
					return false;
				}
			}

			if constexpr (static_cast<bool>(FORMATIONS & Formation::vertical)) {
				if (is_vertical(line)) {
					return false;
				}
			}

			if constexpr (static_cast<bool>(FORMATIONS & Formation::diagonal)) {
				if (is_diagonal(line)) {
					return false;
				}
			}

			return true;
		};

		lines.erase(std::remove_if(lines.begin(), lines.end(), should_be_removed), lines.end());

		return std::move(lines);
	}

	template<size_t FORMATIONS>
	static std::map<Point_t, uint32_t> _calculate_point_densities(std::vector<Line_t> lines)
	{
		auto out = std::map<Point_t, uint32_t>{};

		for (auto& line : lines) {
			for (auto& point : rasterize<FORMATIONS>(line)) {
				out[point]++;
			}
		}

		return out;
	}

	static uint32_t _calculate_score(std::map<Point_t, uint32_t> point_densities)
	{
		return static_cast<uint32_t>(std::count_if(point_densities.begin(), point_densities.end(), [](const auto& point_and_count) {
			return point_and_count.second > 1;
			}));
	}

	std::istream& _data_stream;
};

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T, arma::uword KERNEL_SIZE>
class FloorHeightAnalyser
{
public:

	using Value_t = Value_T;
	using Size_t = arma::uword;

	class Minima
	{
	public:
		using Value_t = Value_T;
		using Size_t = size_t;
		using Point_t = Point3D<Size_t, Size_t, Value_t>;

	private:
		std::vector<Point_t> _points;

	public:

		using Iterator_t = decltype(_points.begin());
		using ConstIterator_t = decltype(_points.cbegin());

		Minima& append(std::vector<Point_t>&& new_points)
		{
			_points.insert(_points.end(), new_points.begin(), new_points.end());

			return *this;
		}

		Minima& append(Minima&& new_minima)
		{
			return append(std::forward<std::vector<Point_t>>(new_minima._points));
		}

		Minima& append(Size_t x, Size_t y, Value_t z)
		{
			_points.emplace_back(x, y, z);

			return *this;
		}

		ConstIterator_t begin() const { return _points.cbegin(); }
		Iterator_t begin() { return _points.begin(); }

		ConstIterator_t end() const { return _points.cend(); }
		Iterator_t end() { return _points.end(); }

		Size_t size() const { return _points.size(); }
	};

	Size_t rows() const { return _height_map.n_rows - 2 * KERNEL_SIZE; }
	Size_t cols() const { return _height_map.n_cols - 2 * KERNEL_SIZE; }

	FloorHeightAnalyser& load(std::istream& is)
	{
		const auto digits = std::string(std::istreambuf_iterator<char>{is}, std::istreambuf_iterator<char>{});
		const auto rows = std::count(digits.begin(), digits.end(), '\n') + 1;
		const auto cols = std::distance(digits.begin(), std::find(digits.begin(), digits.end(), '\n'));

		if (rows == 0 || cols == 0) {
			return *this;
		}

		_height_map.resize(rows + 2 * KERNEL_SIZE, cols + 2 * KERNEL_SIZE);
		_height_map.fill(10);

		auto digit = digits.begin();
		for (arma::uword r = KERNEL_SIZE; r < _height_map.n_rows - KERNEL_SIZE - 1; ++r) {
			std::transform(digit, std::next(digit, cols), std::next(_height_map.begin_row(r), KERNEL_SIZE),
				[](auto d) { return static_cast<uint8_t>(d - '0'); });
			std::advance(digit, cols + 1);
		}

		std::transform(digit, std::next(digit, cols), std::next(_height_map.begin_row(rows - 1), KERNEL_SIZE),
			[](auto d) { return static_cast<uint8_t>(d - '0'); });

		return *this;
	}

	Minima find_minima() const
	{
		auto out = Minima{};

		for (auto col = KERNEL_SIZE; col < _height_map.n_cols - KERNEL_SIZE; ++col) {
			for (auto row = KERNEL_SIZE; row < _height_map.n_rows - KERNEL_SIZE; ++row) {
				const auto ref_value = _height_map.at(row, col);
				if (!_is_minimum(row, col, ref_value)) {
					continue;
				}

				out.append(col, row, ref_value);
			}
		}

		return out;
	}

private:

	bool _is_minimum(Size_t row, Size_t col, Value_t ref_value) const
	{
		if (_height_map.at(row, col - KERNEL_SIZE) <= ref_value) {
			return false;
		}
		if (_height_map.at(row, col + KERNEL_SIZE) <= ref_value) {
			return false;
		}
		if (_height_map.at(row - KERNEL_SIZE, col) <= ref_value) {
			return false;
		}
		if (_height_map.at(row + KERNEL_SIZE, col) <= ref_value) {
			return false;
		}

		return true;
	}

	arma::Mat<Value_t> _height_map;
};

///////////////////////////////////////////////////////////////////////////////

class BoatSystems
{
public:
	template<size_t WINDOW_SIZE, typename Iter_T>
	uint32_t depth_score(Iter_T begin, Iter_T end) const
	{
		auto current_idx = size_t{ 0 };

		auto window = std::array<typename Iter_T::value_type, WINDOW_SIZE>{};
		for (auto it = window.begin(); it < window.end(); ++it, ++current_idx) {
			*it = *begin++;
		}

		auto prev_score = std::accumulate(window.begin(), window.end(), uint32_t{ 0 });

		return std::accumulate(begin, end, 0, [&window, &prev_score, &current_idx](auto curr, auto next) {

			auto new_score = uint32_t{ prev_score - window[current_idx % window.size()] + next };

			auto out = new_score > prev_score ? ++curr : curr;

			prev_score = new_score;
			window[current_idx % window.size()] = next;
			++current_idx;

			return out;
			});
	}

	template<typename Iter_T>
	Direction net_direction(Iter_T begin, Iter_T end) const 
	{
		return std::accumulate(begin, end, Direction{});
	}

	template<typename Iter_T>
	Direction net_aiming(Iter_T begin, Iter_T end) const
	{
		return std::accumulate(begin, end, Aiming{}).to_direction();
	}

	uint32_t power_consumption(const DiagnosticLog& log) const
	{
		return LogProcessor::power_consumption(log);
	}

	uint32_t life_support_rating(const DiagnosticLog& log) const
	{
		return LogProcessor::life_support_rating(log);
	}

	template<size_t FORMATIONS>
	uint32_t detect_vents(std::istream& data) const
	{
		return VentAnalyzer{ data }.score<FORMATIONS>();
	}

	uint32_t lava_tube_smoke_risk(std::istream& data) const
	{
		return static_cast<uint32_t>(FloorHeightAnalyser<uint8_t, 1>{}.load(data).find_minima().size());
	}
};

///////////////////////////////////////////////////////////////////////////////

}	// namespace: aoc

namespace std
{

///////////////////////////////////////////////////////////////////////////////

istream& operator>>(istream& is, aoc::Direction& direction)
{
	auto cmd = std::string{};

	direction = aoc::Direction{ 0, 0 };

	is >> cmd;

	if (cmd.empty())
		return is;

	if (cmd == "forward")
	{
		is >> direction.x;
	}
	else if (cmd == "up")
	{
		is >> direction.y;
		direction.y *= -1;
	}
	else if (cmd == "down")
	{
		is >> direction.y;
	}
	else {
		throw aoc::Exception(std::format("Invalid direction string"));
	}

	if (is.fail())
		throw aoc::Exception(std::format("Invalid direction string"));

	return is;
}

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
