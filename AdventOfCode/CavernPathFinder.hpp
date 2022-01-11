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
	using EdgeWeight_t = boost::property<boost::edge_weight_t, int>;
	using Vertex_t = int;
	using Graph_t = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex_t, EdgeWeight_t>;

	using VertexDescriptor_t = boost::graph_traits<Graph_t>::vertex_descriptor;
	using Route_t = std::vector<VertexDescriptor_t>;
public:

	CavernPathFinder& plot_course(const Cavern::Grid_t& risk_grid)
	{
		_optimal_path = _find_path_via_dijkstra(
			_build_graph(risk_grid),
			VertexDescriptor_t{ 0 },
			VertexDescriptor_t{ risk_grid.n_elem- 1 }
		);

		return *this;
	}

	Route_t route() const { return _optimal_path; }

	size_t score() const { return 0; }

private:
	static Graph_t _build_graph(const Cavern::Grid_t& risk_grid)
	{
		auto out = Graph_t(risk_grid.n_rows * risk_grid.n_cols);

		auto vertex_from_row_col = [&risk_grid](auto r, auto c) -> size_t {
			return r * risk_grid.n_cols + c;
		};

		// Add bulk edges.
		for (auto r = 0u; r < risk_grid.n_rows - 1; ++r) {
			for (auto c = 0u; c < risk_grid.n_cols - 1; ++c) {
				const auto v_0 = vertex_from_row_col(r, c);
				const auto v_down = vertex_from_row_col(r + 1, c);
				const auto v_right = vertex_from_row_col(r, c + 1);

				Logger::WriteMessage(std::format("{} -{}-> {}\n", v_0, risk_grid.at(r + 1, c), v_down).c_str());
				boost::add_edge(v_0, v_down, EdgeWeight_t{ risk_grid.at(r + 1, c) }, out);

				Logger::WriteMessage(std::format("{} -{}-> {}\n", v_0, risk_grid.at(r, c + 1), v_right).c_str());
				boost::add_edge(v_0, v_right, EdgeWeight_t{ risk_grid.at(r, c + 1) }, out);
			}
		}

		// Add edges down right side of grid.
		const auto final_col_idx = risk_grid.n_cols - 1;
		for (auto r = 0u; r < risk_grid.n_rows - 1; ++r) {
			const auto v_0 = vertex_from_row_col(r, final_col_idx);
			const auto v_down = vertex_from_row_col(r + 1, final_col_idx);

			Logger::WriteMessage(std::format("{} -{}-> {}\n", v_0, risk_grid.at(r + 1, final_col_idx), v_down).c_str());
			boost::add_edge(vertex_from_row_col(r, final_col_idx), vertex_from_row_col(r + 1, final_col_idx), EdgeWeight_t{ risk_grid.at(r + 1, final_col_idx) }, out);
		}

		// Add edges along bottom of grid.
		const auto final_row_idx = risk_grid.n_rows - 1;
		for (auto c = 0u; c < risk_grid.n_cols - 1; ++c) {
			const auto v_0 = vertex_from_row_col(final_row_idx, c);
			const auto v_right = vertex_from_row_col(final_row_idx, c + 1);

			Logger::WriteMessage(std::format("{} -{}-> {}\n", v_0, risk_grid.at(final_row_idx, c + 1), v_right).c_str());
			boost::add_edge(vertex_from_row_col(final_row_idx, c), vertex_from_row_col(final_row_idx, c + 1), EdgeWeight_t{ risk_grid.at(final_row_idx, c + 1) }, out);
		}

		return std::move(out);
	}

	static Route_t _find_path_via_dijkstra(const Graph_t& cavern_graph, const VertexDescriptor_t& start_vertex, const VertexDescriptor_t& end_vertex)
	{
		using namespace boost;
		

		const auto vertex_count = num_vertices(cavern_graph);
		auto distances = std::vector<int>(vertex_count);
		auto route_map = std::vector<VertexDescriptor_t>(vertex_count);

		auto distance_map = predecessor_map(make_iterator_property_map( route_map.begin(), get(vertex_index, cavern_graph)))
			.distance_map(make_iterator_property_map(distances.begin(), get(vertex_index, cavern_graph)));

		boost::dijkstra_shortest_paths(cavern_graph, start_vertex, distance_map);

		return _get_forward_path(cavern_graph, route_map, start_vertex, end_vertex);
	}

	static Route_t _get_forward_path(const Graph_t& cavern_graph, const Route_t& route_map, const VertexDescriptor_t& start_vertex, const VertexDescriptor_t& end_vertex)
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


	Route_t _optimal_path;
};

///////////////////////////////////////////////////////////////////////////////

}
}

///////////////////////////////////////////////////////////////////////////////
