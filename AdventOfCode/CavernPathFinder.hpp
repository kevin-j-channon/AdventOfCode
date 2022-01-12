#pragma once

#include "StringOperations.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

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
				target.at(target_row, c) = _apply_offset(offset, _risk_grid.at(r, c));
			}
		}
	}

	int _offset_from_row_and_col(Size_t row, Size_t col) const
	{
		const auto block_row = row / _risk_grid.n_rows;
		const auto block_col = col / _risk_grid.n_cols;

		return block_row + block_col;
	}

	static int _apply_offset(int offset, int value)
	{
		return (value + offset) % 10;
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
	using Route_t = std::vector<VertexDescriptor_t>;
public:

	CavernPathFinder& plot_course(const Cavern::Grid_t& risk_grid)
	{
		_graph = _build_graph(risk_grid);
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
			out += boost::get(boost::edge_weight, _graph, _get_edge(*src, *dst));
		}

		return out;
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

	static Graph_t _build_graph(const Cavern::Grid_t& risk_grid)
	{
		auto out = Graph_t(risk_grid.n_elem);

		auto vertex_from_row_col = [&risk_grid](auto r, auto c) -> size_t {
			return r * risk_grid.n_cols + c;
		};

		// Add bulk edges.
		for (auto r = 0u; r < risk_grid.n_rows - 1; ++r) {
			for (auto c = 0u; c < risk_grid.n_cols - 1; ++c) {
				const auto v_0 = vertex_from_row_col(r, c);

				boost::add_edge(v_0, vertex_from_row_col(r + 1, c), EdgeWeight_t{ risk_grid.at(r + 1, c) }, out);
				boost::add_edge(v_0, vertex_from_row_col(r, c + 1), EdgeWeight_t{ risk_grid.at(r, c + 1) }, out);
			}
		}

		// Add edges down right side of grid.
		const auto final_col_idx = risk_grid.n_cols - 1;
		for (auto r = 0u; r < risk_grid.n_rows - 1; ++r) {
			boost::add_edge(vertex_from_row_col(r, final_col_idx), vertex_from_row_col(r + 1, final_col_idx), EdgeWeight_t{ risk_grid.at(r + 1, final_col_idx) }, out);
		}

		// Add edges along bottom of grid.
		const auto final_row_idx = risk_grid.n_rows - 1;
		for (auto c = 0u; c < risk_grid.n_cols - 1; ++c) {
			boost::add_edge(vertex_from_row_col(final_row_idx, c), vertex_from_row_col(final_row_idx, c + 1), EdgeWeight_t{ risk_grid.at(final_row_idx, c + 1) }, out);
		}

		return std::move(out);
	}

	Route_t _find_path_via_dijkstra(const VertexDescriptor_t& start_vertex, const VertexDescriptor_t& end_vertex)
	{
		using namespace boost;
		

		const auto vertex_count = num_vertices(_graph);
		auto distances = std::vector<int>(vertex_count);
		auto route_map = std::vector<VertexDescriptor_t>(vertex_count);

		auto distance_map = predecessor_map(make_iterator_property_map( route_map.begin(), get(vertex_index, _graph)))
			.distance_map(make_iterator_property_map(distances.begin(), get(vertex_index, _graph)));

		boost::dijkstra_shortest_paths(_graph, start_vertex, distance_map);

		return _get_forward_path(route_map, start_vertex, end_vertex);
	}

	static Route_t _get_forward_path(const Route_t& route_map, const VertexDescriptor_t& start_vertex, const VertexDescriptor_t& end_vertex)
	{
		auto path = Route_t{};
		path.reserve(route_map.size());

		for (auto current = end_vertex; current != start_vertex; current = route_map[current]) {
			path.push_back(current);
		}

		path.push_back(start_vertex);

		std::reverse(path.begin(), path.end());
		
		return std::move(path);
	}

	Graph_t _graph;
	Route_t _optimal_path;
};

///////////////////////////////////////////////////////////////////////////////

}
}

///////////////////////////////////////////////////////////////////////////////
