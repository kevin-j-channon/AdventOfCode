#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "Common.hpp"
#include "StringOperations.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

using Cave_t = std::string;
using Tunnel_t = std::pair<Cave_t, Cave_t>;

using EdgeWeightProperty_t = boost::property<boost::edge_weight_t, int>;
using CaveMap_t = boost::labeled_graph<boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Cave_t, EdgeWeightProperty_t>, std::string>;

using Route_t = std::vector<Cave_t>;

///////////////////////////////////////////////////////////////////////////////

namespace route
{
inline std::string as_string(const Route_t& route)
{
	return join(route, ',');
}
}

///////////////////////////////////////////////////////////////////////////////

struct TerminalCaveType
{
	enum Type
	{
		none = 0b0,
		start = 0b1,
		end = 0b10,
	};

	template<size_t type_T>
	static constexpr const char* name() {
		static_assert(false, "Invalid terminal cave type");
	}

	template<>
	static constexpr const char* name<start>() { return "start"; }

	template<>
	static constexpr const char* name<end>() { return "end"; }

	static int to_type(Tunnel_t tunnel)
	{
		if (tunnel.first == TerminalCaveType::name<TerminalCaveType::start>()
			|| tunnel.second == TerminalCaveType::name<TerminalCaveType::start>()) {

			if (tunnel.first == TerminalCaveType::name<TerminalCaveType::end>()
				|| tunnel.second == TerminalCaveType::name<TerminalCaveType::end>()) {
				return start | end;
			}
			
			return start;
		}

		if (tunnel.first == TerminalCaveType::name<TerminalCaveType::end>()
			|| tunnel.second == TerminalCaveType::name<TerminalCaveType::end>()) {
			return end;
		}

		return none;
	}
};

///////////////////////////////////////////////////////////////////////////////

class CaveMapBuilder
{

	CaveMap_t& _cave_map;
public:

	template<typename TunnelIter_T>
	struct TunnelConnectionPartition
	{
		using Range_t = std::ranges::subrange<TunnelIter_T>;

		Range_t starts;
		Range_t ends;
		Range_t others;
	};

	CaveMapBuilder(CaveMap_t& cave_map)
		: _cave_map{cave_map}
	{}

	template<size_t TYPE, typename TunnelIter_T>
	CaveMapBuilder& handle_terminal_cave(TunnelIter_T begin, TunnelIter_T end)
	{
		const auto terminal_vertex = _cave_map.insert_vertex(TerminalCaveType::name<TYPE>());
		if (terminal_vertex.second) {
			_cave_map[TerminalCaveType::name<TYPE>()] = TerminalCaveType::name<TYPE>();
			Logger::WriteMessage(std::format("Added cave '{}' (vertex {})\n", TerminalCaveType::name<TYPE>(), _cave_map.vertex(TerminalCaveType::name<TYPE>())).c_str());
		}

		std::for_each(begin, end, [&](const auto& tunnel) {
			const auto name = tunnel.first == TerminalCaveType::name<TYPE>() ? tunnel.second : tunnel.first;
			const auto non_terminal_vertex = _cave_map.insert_vertex(name);
			if (non_terminal_vertex.second) {
				_cave_map[name] = name;
				Logger::WriteMessage(std::format("Added cave '{}' (vertex {})\n", name, _cave_map.vertex(name)).c_str());
			}

			if constexpr (TYPE == TerminalCaveType::start) {
				// If we're adding "start" caves, then the edges are directed out of the terminal (start) vertex.
				boost::add_edge(terminal_vertex.first, non_terminal_vertex.first, 1, _cave_map);

				Logger::WriteMessage(std::format("Added edge {} -> {}\n", _cave_map.graph()[terminal_vertex.first], _cave_map.graph()[non_terminal_vertex.first]).c_str());
			}
			else {
				// If we're adding "end" caves, then the edges are directed into the terminal (end) vertex.
				boost::add_edge(non_terminal_vertex.first, terminal_vertex.first, 1, _cave_map);

				Logger::WriteMessage(std::format("Added edge {} -> {}\n", _cave_map.graph()[non_terminal_vertex.first], _cave_map.graph()[terminal_vertex.first]).c_str());
			}
			});

		return *this;
	}

	template<typename TunnelIter_T>
	CaveMapBuilder& add_non_terminal_tunnels(TunnelIter_T begin, TunnelIter_T end)
	{
		std::for_each(begin, end, [this](auto tunnel) {
			const auto cave_1 = _cave_map.insert_vertex(tunnel.first);
			_cave_map[tunnel.first] = tunnel.first;
			if (cave_1.second)
				Logger::WriteMessage(std::format("Added cave '{}' (vertex {})\n", tunnel.first, _cave_map.vertex(tunnel.first)).c_str());

			const auto cave_2 = _cave_map.insert_vertex(tunnel.second);
			_cave_map[tunnel.second] = tunnel.second;
			if (cave_2.second)
				Logger::WriteMessage(std::format("Added cave '{}' (vertex {})\n", tunnel.second, _cave_map.vertex(tunnel.second)).c_str());

			// We want to be able to go into and out of non-terminal caves.
			boost::add_edge(cave_1.first, cave_2.first, 1, _cave_map);
			Logger::WriteMessage(std::format("Added edge {} -> {}\n", _cave_map.graph()[cave_1.first], _cave_map.graph()[cave_2.first]).c_str());

			boost::add_edge(cave_2.first, cave_1.first, 1, _cave_map);
			Logger::WriteMessage(std::format("Added edge {} -> {}\n", _cave_map.graph()[cave_2.first], _cave_map.graph()[cave_1.first]).c_str());
			});

		return *this;
	}

	template<typename TunnelContainer_T>
	static TunnelConnectionPartition<typename TunnelContainer_T::iterator> partition_tunnels(TunnelContainer_T& tunnels)
	{
		const auto end_of_starts = std::partition(tunnels.begin(), tunnels.end(), [](const auto& t) {
			return 0 != (TerminalCaveType::to_type(t) & TerminalCaveType::start);
			});

		const auto end_of_ends = std::partition(end_of_starts, tunnels.end(), [](const auto& t) {
			return 0 != (TerminalCaveType::to_type(t) & TerminalCaveType::end);
			});

		return { 
			{tunnels.begin(), end_of_starts},
			{end_of_starts, end_of_ends},
			{end_of_ends, tunnels.end()} };
	}
};

///////////////////////////////////////////////////////////////////////////////

class CavePaths : public std::ranges::view_interface<CavePaths>
{
public:

	auto begin() const { return _paths.begin(); }
	auto end() const { return _paths.end(); }

	void add_path(std::string path)
	{
		_paths.push_back(std::move(path));
	}

private:
	std::vector< std::string > _paths;
};

///////////////////////////////////////////////////////////////////////////////

class RouteIterator
{
	enum class RouteStatus
	{
		exploring,
		dead_end,
		found_end
	};

public:

	RouteIterator(const CaveMap_t& caves)
		: _caves{ caves }
	{
		Logger::WriteMessage("\nFinding initial route\n");
		_current_route.emplace_back("start", 0);
		Logger::WriteMessage(std::format("Adding 'start' (vertex {}) to route with current tunnel {}\n", _caves.vertex("start"), 0).c_str());

		_recurse_through_tunnels(Cave_t{ "start" });
	}

	Route_t operator*() const
	{
		if (_current_route.empty()) {
			throw OutOfRangeException("Route iterator is at end");
		}

		auto route = Route_t(_current_route.size());

		std::transform(_current_route.begin(), _current_route.end(), route.begin(), [](const auto& breadcrumb) {
			return breadcrumb.first;
			});

		return route;
	}

	RouteIterator& operator++()
	{
		Logger::WriteMessage("\noperator++\n");

		_rewind_to_next_unexplored_tunnel();

		while (true) {
			if (_current_route.empty()) {
				return *this;
			}

			switch (_explore_next_unexplored_tunnel()) {
			case RouteStatus::exploring: {
				throw Exception("Exploring next unexplored tunnel failed to reach a conclusion");
			}
			case RouteStatus::dead_end: {
				Logger::WriteMessage("operator++ found a dead-end. Looking at previous cave\n");
				continue;
			}
			case RouteStatus::found_end: {
				Logger::WriteMessage("operator++ found the end cave. Getting out\n");
				Logger::WriteMessage(std::format("FOUND ROUTE: {}\n", route::as_string(this->operator*())).c_str());
				return *this;
			}
			default: {
				throw Exception("Unhandled result of exploring next tunnel");
			}
			}
		}

		Logger::WriteMessage(std::format("Incremented to route: {}\n", route::as_string(this->operator*())).c_str());

		return *this;
	}

private:
	using Breadcrumb_t = std::vector<std::pair<Cave_t, int>>;

	const CaveMap_t& _caves;
	std::set<Cave_t> _not_revisitable;
	Breadcrumb_t _current_route;

	static bool _is_singly_visitable(const Cave_t& cave)
	{
		if (cave == "start" || cave == "end") {
			return true;
		}

		return std::all_of(cave.begin(), cave.end(), [](auto c) { return std::islower(c); });
	}

	void _debug_print_breadcrumbs() const
#ifdef _DEBUG
	{
		Logger::WriteMessage("Current state of breadcrumbs:\n");
		for (const auto& bc : _current_route) {
			Logger::WriteMessage(std::format("\t{}, {}\n", bc.first, bc.second).c_str());
		}
	}
#else
	{}
#endif

	void _debug_print_edges(const Cave_t& cave) const
#ifdef _DEBUG
	{
		auto i = int{ 0 };
		Logger::WriteMessage(std::format("Cave '{}' has out edges:\n", cave).c_str());
		for (auto [edge, edges_end] = boost::out_edges(_caves.vertex(cave), _caves); edge != edges_end; ++edge, ++i) {
			Logger::WriteMessage(std::format("\tEdge {}: {} -> {}\n", i, boost::source(*edge, _caves), boost::target(*edge, _caves)).c_str());
		}
	}
#else
	{}
#endif

	RouteStatus _recurse_through_tunnels(const Cave_t& cave)
	{
		Logger::WriteMessage("Recursing through tunnels\n");
		_debug_print_breadcrumbs();

		if (cave == "end") {
			Logger::WriteMessage("This was the end cave\n");
			return RouteStatus::found_end;
		}

		if (_is_singly_visitable(cave)) {
			Logger::WriteMessage("This cave is not revisitable\n");
			_not_revisitable.insert(cave);
		}

		auto [edge, edges_end] = boost::out_edges(_caves.vertex(cave), _caves);
		_debug_print_edges(cave);

		// Move to the next unexplored tunnel from this cave.
		Logger::WriteMessage(std::format("Advancing to explore tunnel {} from this cave\n", _current_route.back().second).c_str());
		std::advance(edge, _current_route.back().second);

		for (; edge != edges_end; ++edge) {

			const auto& next_cave = _caves.graph()[boost::target(*edge, _caves)];
			Logger::WriteMessage(std::format("The next cave to explore is '{}' (vertex {}), via tunnel {}\n", next_cave, boost::target(*edge, _caves), _current_route.back().second).c_str());

			_current_route.back().second += 1;
			Logger::WriteMessage(std::format("Next time we visit {} we'll be looking at tunnel {}\n", _current_route.back().first, _current_route.back().second).c_str());

			if (_not_revisitable.contains(next_cave)) {
				Logger::WriteMessage("Wait! That cave isn't revisitable. Moving on\n");
				continue;
			}

			Logger::WriteMessage(std::format("Recursing into {}\n", next_cave).c_str());

			_current_route.emplace_back(next_cave, 0);
			Logger::WriteMessage(std::format("Adding '{}' (vertex {}) to route with current tunnel {}\n", next_cave, _caves.vertex(next_cave), 0).c_str());

			const auto status = _recurse_through_tunnels(next_cave);
			if (status == RouteStatus::found_end) {
				// We found the end, get out and let the caller know (it could be ourself).
				Logger::WriteMessage("Found the end. Getting out\n");
				return status;
			}

			// Explore the next tunnel now.
		}

		// We explored all the tunnels from this cave and didn't find the end. This cave was en route to a dead-end; remove
		// from the stack and let the caller know.
		Logger::WriteMessage(std::format("Turns out that {} leads to a dead-end. Removing it from the route and trying something else\n", _current_route.back().first).c_str());
		_pop_route();
		return RouteStatus::dead_end;
	}

	void _pop_route()
	{
		Logger::WriteMessage("Popping route\n");
		auto non_revisitable_cave = _not_revisitable.find(_current_route.back().first);
		if (_not_revisitable.end() != non_revisitable_cave) {
			_not_revisitable.erase(non_revisitable_cave);
		}

		_debug_print_breadcrumbs();

		if (!_current_route.empty()) {
			_current_route.pop_back();
			if (!_current_route.empty()) {
				Logger::WriteMessage(std::format("Top route is now ({},{})\n", _current_route.back().first, _current_route.back().second).c_str());
			}
		}
		else {
			Logger::WriteMessage("No routes left\n");
		}
	}

	void _rewind_to_next_unexplored_tunnel()
	{
		Logger::WriteMessage("Rewinding to next un-explored tunnel\n");
		_pop_route();
		if (_current_route.empty()) {
			return;
		}

		const auto [edges_begin, edges_end] = boost::out_edges(_caves.vertex(_current_route.back().first), _caves);
		const auto num_out_edges = std::distance(edges_begin, edges_end);

		if (_current_route.back().second < num_out_edges) {
			Logger::WriteMessage(std::format("Cave {} still has unexplored tunnel {}\n", _current_route.back().first, _current_route.back().second).c_str());
			return;
		}

		// Recurse down the stack of caves
		Logger::WriteMessage("Recursing to previous cave\n");
		_rewind_to_next_unexplored_tunnel();
	}

	RouteStatus _explore_next_unexplored_tunnel()
	{
		Logger::WriteMessage(std::format("Exploring unexplored tunnel {} from cave {}\n", _current_route.back().second, _current_route.back().first).c_str());

		_debug_print_breadcrumbs();

		auto [edge, edges_end] = boost::out_edges(_caves.vertex(_current_route.back().first), _caves);
		auto num_out_edges = std::distance(edge, edges_end);

		while (_current_route.back().second >= num_out_edges) {
			Logger::WriteMessage(std::format("All tunnels from Cave {} have been explored. Moving to previous cave\n", _current_route.back().first).c_str());
			_pop_route();
			if (_current_route.empty()) {
				Logger::WriteMessage(std::format("No routes left\n").c_str());
				return RouteStatus::dead_end;
			}

			_debug_print_breadcrumbs();

			std::tie(edge, edges_end) = boost::out_edges(_caves.vertex(_current_route.back().first), _caves);
			num_out_edges = std::distance(edge, edges_end);
		}

		auto i = int{ 0 };
		Logger::WriteMessage(std::format("Cave '{}' has out edges:\n", _current_route.back().first).c_str());
		for (auto e2 = edge; e2 != edges_end; ++e2, ++i) {
			Logger::WriteMessage(std::format("\tEdge {}: {} -> {}\n", i, boost::source(*e2, _caves), boost::target(*e2, _caves)).c_str());
		}

		std::advance(edge, _current_route.back().second);

		auto next_cave = _caves.graph()[boost::target(*edge, _caves)];
		_current_route.back().second += 1;
		Logger::WriteMessage(std::format("Incrementing to tunnel {} (goes to cave {}) as the next unexplored tunnel for cave {}\n", _current_route.back().second, next_cave, _current_route.back().first).c_str());

		while (_not_revisitable.contains(next_cave)) {
			if (++edge == edges_end) {
				Logger::WriteMessage("Failed to find any visitable caves. This is a dead-end\n");
				_pop_route();
				return RouteStatus::dead_end;
			}

			Logger::WriteMessage(std::format("Cave '{}' is not revisitable, checking next tunnel\n", next_cave).c_str());

			next_cave = _caves.graph()[boost::target(*edge, _caves)];
			_current_route.back().second += 1;
			Logger::WriteMessage(std::format("Incrementing to tunnel {} (goes to cave {}) as the next unexplored tunnel for cave {}\n", _current_route.back().second, next_cave, _current_route.back().first).c_str());
		}

		_current_route.emplace_back(next_cave, 0);
		return _recurse_through_tunnels(next_cave);
	}
};

///////////////////////////////////////////////////////////////////////////////

class CaveNavigator
{
public:

	const CaveMap_t& cave_map() const { return _cave_system; }

	CaveNavigator& load(std::istream& is)
	{
		auto tunnels = _load_tunnels(is);

		const auto partitions = CaveMapBuilder::partition_tunnels(tunnels);

		CaveMapBuilder{ _cave_system }
			.handle_terminal_cave<TerminalCaveType::start>(partitions.starts.begin(), partitions.starts.end())
			.handle_terminal_cave<TerminalCaveType::end>(partitions.ends.begin(), partitions.ends.end())
			.add_non_terminal_tunnels(partitions.others.begin(), partitions.others.end());

		return *this;
	}

	CavePaths paths()
	{
		return CavePaths{};
	}

private:

	static std::vector<Tunnel_t> _load_tunnels(std::istream& is)
	{
		auto tunnels = std::vector<Tunnel_t>{};
		while (!is.eof()) {
			tunnels.push_back(_load_tunnel(is));
		}

		return tunnels;
	}

	static Tunnel_t _load_tunnel(std::istream& is)
	{
		auto line = _get_line_from_stream(is);
		return _get_tunnel_from_string(std::move(line));
	}

	static std::string _get_line_from_stream(std::istream& is)
	{
		auto line = std::string{};
		std::getline(is, line);
		if (is.fail()) {
			throw Exception("Failed to read edge from file");
		}

		return line;
	}

	static Tunnel_t _get_tunnel_from_string(std::string s)
	{
		const auto caves = split(s, '-');
		if (caves.size() != 2) {
			throw Exception(std::format("Invalid tunnel line: {}", s));
		}

		return { caves[0], caves[1] };
	}

	CaveMap_t _cave_system;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
