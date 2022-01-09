#pragma once

#include "StringOperations.hpp"

#include <boost/graph/adjacency_list.hpp>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
namespace navigation
{

///////////////////////////////////////////////////////////////////////////////

class Cavern : boost::noncopyable
{
public:
	using Grid_t = arma::Mat<int>;
	using Size_t = arma::uword;

	Cavern() {}

	Cavern(Cavern&&) = default;
	Cavern& operator=(Cavern&&) = default;

	Cavern(std::istream& is)
	{
		_risk_grid = _read_risk_grid(is);
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

class CavernPathFinder : boost::noncopyable
{
	using EdgeWeightProperty_t = boost::property<boost::edge_weight_t, int>;
	using Graph_t = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, int, EdgeWeightProperty_t>;
public:

	CavernPathFinder& plot_course(const Cavern& cavern)
	{
		return *this;
	}

private:
	static Graph_t _build_graph(const Cavern::Grid_t& risk_grid)
	{
		return Graph_t{};
	}
};

///////////////////////////////////////////////////////////////////////////////

}
}

///////////////////////////////////////////////////////////////////////////////
