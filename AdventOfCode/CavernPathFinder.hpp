#pragma once

#include "StringOperations.hpp"

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
namespace navigation
{

///////////////////////////////////////////////////////////////////////////////

class Cavern
{
public:
	using Grid_t = arma::Mat<int>;
	using Size_t = arma::uword;

	static Cavern from_stream(std::istream& is)
	{
		auto out = Cavern{};
		out._risk_grid = _read_risk_grid(is);

		return out;
	}

	const Grid_t& risk_grid() const
	{
		return _risk_grid;
	}

private:
	static Grid_t _read_risk_grid(std::istream& is)
	{
		auto line = std::string{};
		
		std::getline(is, line);
		auto out = Grid_t(line.length(), line.length());
		auto out_it = _copy_into_grid(line, out.begin());

		while (std::getline(is, line)) {
			out_it = _copy_into_grid(line, out_it);
		}

		return out.t();
	}

	static Grid_t::iterator _copy_into_grid(const std::string& str, Grid_t::iterator it)
	{
		return std::transform(str.begin(), str.end(), it, [](auto c) {
			return static_cast<int>(c - '0');
			});
	}

	Grid_t _risk_grid;
};

///////////////////////////////////////////////////////////////////////////////

}
}

///////////////////////////////////////////////////////////////////////////////
