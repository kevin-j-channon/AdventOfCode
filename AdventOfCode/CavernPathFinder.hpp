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
	using EdgeWeight_t = boost::property<boost::edge_weight_t, int>;
	using Vertex_t = int;
	using Graph_t = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex_t, EdgeWeight_t>;

	using VertexDescriptor_t = boost::graph_traits<Graph_t>::vertex_descriptor;
	using Route_t = std::vector<VertexDescriptor_t>;
public:

	CavernPathFinder& plot_course(const Cavern& cavern)
	{
		const auto cavern_graph = _build_graph(cavern.risk_grid());

		const auto start_vertex = VertexDescriptor_t{ 0 };
		const auto end_vertex = VertexDescriptor_t{ cavern.risk_grid().n_cols * cavern.risk_grid().n_rows };
		const auto optimal_path = _find_path_via_dijkstra(cavern_graph, start_vertex, end_vertex);

		return *this;
	}

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
				boost::add_edge(vertex_from_row_col(r, c), vertex_from_row_col(r + 1, c), EdgeWeight_t{ risk_grid.at(r + 1, c) }, out);
				boost::add_edge(vertex_from_row_col(r, c), vertex_from_row_col(r, c + 1), EdgeWeight_t{ risk_grid.at(r, c + 1) }, out);
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

	static Route_t _find_path_via_dijkstra(const Graph_t& cavern_graph, const VertexDescriptor_t& start_vertex, const VertexDescriptor_t& end_vertex)
	{
		const auto vertex_count = boost::num_vertices(cavern_graph);
		auto distances = std::vector<int>(vertex_count);
		auto pMap = std::vector<VertexDescriptor_t>(vertex_count);

		auto distanceMap = boost::predecessor_map(
			boost::make_iterator_property_map(pMap.begin(), boost::get(boost::vertex_index, cavern_graph))).distance_map(
				boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, cavern_graph)));

		boost::dijkstra_shortest_paths(cavern_graph, start_vertex, distanceMap);
		return _getPath(graph, pMap, source, destination);
	}

	static Route_t _getPath(
		const DirectedGraphType& graph,
		const std::vector<VertexDescriptor>& pMap,
		const VertexDescriptor& source,
		const VertexDescriptor& destination
	) {
		std::vector<VertexDescriptor> path;
		VertexDescriptor current = destination;
		while (current != source)
		{
			path.push_back(current);
			current = pMap[current];
		}
		path.push_back(source);
		return path;
	}
};

///////////////////////////////////////////////////////////////////////////////

}
}

///////////////////////////////////////////////////////////////////////////////
