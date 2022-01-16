#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

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

	Cavern() {}

	Cavern(const Cavern&) = default;
	Cavern& operator=(const Cavern&) = default;

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

	Cavern& expand(Size_t factor)
	{
		auto new_grid = Grid_t(_risk_grid.n_rows * factor, _risk_grid.n_cols * factor);

		for (auto block_row = 0u; block_row < factor; ++block_row) {
			for (auto block_col = 0u; block_col < factor; ++block_col) {
				_apply_scaled_grid(_row_from_block_row(block_row), _col_from_block_col(block_col), new_grid);
			}
		}

		std::swap(new_grid, _risk_grid);

		return *this;
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

	Size_t _row_from_block_row(Size_t block_row) const
	{
		return block_row * _risk_grid.n_rows;
	}

	Size_t _col_from_block_col(Size_t block_col) const
	{
		return block_col * _risk_grid.n_cols;
	}

	void _apply_scaled_grid(Size_t start_row, Size_t start_col, Grid_t& target) const
	{
		const auto offset = _offset_from_row_and_col(start_row, start_col);
		for (auto r = 0u; r < _risk_grid.n_rows; ++r) {
			const auto target_row = r + start_row;
			for (auto c = 0u; c < _risk_grid.n_cols; ++c) {
				target.at(target_row, c + start_col) = _apply_offset(offset, _risk_grid.at(r, c));
			}
		}
	}

	int _offset_from_row_and_col(Size_t row, Size_t col) const
	{
		const auto block_row = row / _risk_grid.n_rows;
		const auto block_col = col / _risk_grid.n_cols;

		return static_cast<int>(block_row + block_col);
	}

	static int _apply_offset(int offset, int value)
	{
		return (value + offset - 1) % 9 + 1;
	}

	Grid_t _risk_grid;
};

///////////////////////////////////////////////////////////////////////////////

class CavernPathFinder : boost::noncopyable
{
	using EdgeWeight_t = boost::property<boost::edge_weight_t, int>;
	using Vertex_t = int;
	using Graph_t = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex_t, EdgeWeight_t>;

	using VertexDescriptor_t = boost::graph_traits<Graph_t>::vertex_descriptor;
	using VertexRoute_t = std::vector<VertexDescriptor_t>;

	enum Neighbour
	{
		up    = 0b0001,
		down  = 0b0010,
		left  = 0b0100,
		right = 0b1000
	};

public:

	using Point_t = Point2D<Cavern::Size_t>;
	using Route_t = std::vector<Point_t>;

	CavernPathFinder& plot_course(const Cavern::Grid_t& risk_grid)
	{
		_graph = build_graph(risk_grid);
_optimal_path = _find_path_via_dijkstra(
	VertexDescriptor_t{ 0 },
	VertexDescriptor_t{ risk_grid.n_elem - 1 }
);

return *this;
	}

	Route_t route() const { return _optimal_path; }

	size_t score() const
	{
		auto out = size_t{ 0 };

		for (auto src = _optimal_path.begin(), dst = std::next(src); dst != _optimal_path.end(); ++src, ++dst) {
			out += boost::get(boost::edge_weight, _graph, _get_edge(vertex_from_cavern_location(*src), vertex_from_cavern_location(*dst)));
		}

		return out;
	}

	Graph_t build_graph(const Cavern::Grid_t& risk_grid)
	{
		_cavern_rows = risk_grid.n_rows;
		_cavern_cols = risk_grid.n_cols;

		auto out = Graph_t(risk_grid.n_elem);

		_add_edges<up|down|left|right>(risk_grid, out);	// Bulk edges
		_add_edges<up|down|     right>(risk_grid, out);	// Left-most column
		_add_edges<up|down|left      >(risk_grid, out);	// Right-most column
		_add_edges<   down|left|right>(risk_grid, out);	// Top row
		_add_edges<up|     left|right>(risk_grid, out);	// Bottom row
		_add_edges<   down|     right>(risk_grid, out);	// Top-left grid position
		_add_edges<   down|left      >(risk_grid, out);	// Top-right grid position
		_add_edges<up|          right>(risk_grid, out);	// Bottom-left grid position
		_add_edges<up|     left      >(risk_grid, out);	// Bottom-right grid position

		return std::move(out);
	}

	template<int HAS_NEIGHBOUR>
	static constexpr int _row_col_offset()
	{
		if constexpr (HAS_NEIGHBOUR != 0) {
			return 1;
		}
		else {
			return 0;
		}
	}

	template<int NEIGHBOURS>
	void _add_edges(const Cavern::Grid_t& risk_grid, Graph_t& graph)
	{
		auto vertex_from_row_col = [&risk_grid](auto r, auto c) -> size_t {
			return r * risk_grid.n_cols + c;
		};

		const auto row_start = 0u + _row_col_offset<NEIGHBOURS & up>();
		const auto row_end = risk_grid.n_rows - _row_col_offset<NEIGHBOURS & down>();
		const auto col_start = 0u + _row_col_offset<NEIGHBOURS & left>();
		const auto col_end = risk_grid.n_cols - _row_col_offset<NEIGHBOURS & right>();

		// Add bulk edges.
		for (auto r = row_start; r < row_end; ++r) {
			for (auto c = col_start; c < col_end; ++c) {
				const auto v_0 = vertex_from_row_col(r, c);

				if constexpr ((NEIGHBOURS & up) != 0) {
					boost::add_edge(v_0, vertex_from_row_col(r - 1, c), EdgeWeight_t{ risk_grid.at(r - 1, c) }, graph);
				}
				if constexpr ((NEIGHBOURS & down) != 0) {
					boost::add_edge(v_0, vertex_from_row_col(r + 1, c), EdgeWeight_t{ risk_grid.at(r + 1, c) }, graph);
				}
				if constexpr ((NEIGHBOURS & left) != 0) {
					boost::add_edge(v_0, vertex_from_row_col(r, c - 1), EdgeWeight_t{ risk_grid.at(r, c - 1) }, graph);
				}
				if constexpr ((NEIGHBOURS & right) != 0) {
					boost::add_edge(v_0, vertex_from_row_col(r, c + 1), EdgeWeight_t{ risk_grid.at(r, c + 1) }, graph);
				}
			}
		}
	}

	Point_t cavern_location_from_vertex(const VertexDescriptor_t& v) const
	{
		return { v % _cavern_cols, v / _cavern_rows };
	}

	VertexDescriptor_t vertex_from_cavern_location(const Point_t& p) const
	{
		return static_cast<int>(p.y * _cavern_cols + p.x);
	}

private:

	Graph_t::edge_descriptor _get_edge(const VertexDescriptor_t& source, const VertexDescriptor_t& destination) const
	{
		auto [edge, is_present] = boost::edge(source, destination, _graph);
		if (!is_present) {
			throw Exception(std::format("{} -> {} is not a valid edge", source, destination));
		}

		return std::move(edge);
	}

	Route_t _find_path_via_dijkstra(const VertexDescriptor_t& start_vertex, const VertexDescriptor_t& end_vertex)
	{
		using namespace boost;

		const auto vertex_count = num_vertices(_graph);
		auto distances = std::vector<int>(vertex_count);
		auto route_map = VertexRoute_t(vertex_count);

		auto distance_map = predecessor_map(make_iterator_property_map( route_map.begin(), get(vertex_index, _graph)))
			.distance_map(make_iterator_property_map(distances.begin(), get(vertex_index, _graph)));

		boost::dijkstra_shortest_paths(_graph, start_vertex, distance_map);

		return _get_forward_path(route_map, start_vertex, end_vertex);
	}

	Route_t _get_forward_path(const VertexRoute_t& route_map, const VertexDescriptor_t& start_vertex, const VertexDescriptor_t& end_vertex) const
	{
		auto path = Route_t{};
		path.reserve(route_map.size());

		for (auto current = end_vertex; current != start_vertex; current = route_map[current]) {
			path.push_back(cavern_location_from_vertex(current));
		}

		path.push_back({ 0, 0 });

		std::reverse(path.begin(), path.end());
		
		return std::move(path);
	}

	Graph_t _graph;
	Route_t _optimal_path;
	Cavern::Size_t _cavern_rows;
	Cavern::Size_t _cavern_cols;
};

///////////////////////////////////////////////////////////////////////////////

}
}

///////////////////////////////////////////////////////////////////////////////
