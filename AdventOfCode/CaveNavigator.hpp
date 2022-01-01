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
		}

		std::for_each(begin, end, [&](const auto& tunnel) {
			const auto name = tunnel.first == TerminalCaveType::name<TYPE>() ? tunnel.second : tunnel.first;
			const auto non_terminal_vertex = _cave_map.insert_vertex(name);
			if (non_terminal_vertex.second) {
				_cave_map[name] = name;
			}

			if constexpr (TYPE == TerminalCaveType::start) {
				// If we're adding "start" caves, then the edges are directed out of the terminal (start) vertex.
				boost::add_edge(terminal_vertex.first, non_terminal_vertex.first, 1, _cave_map);
			}
			else {
				// If we're adding "end" caves, then the edges are directed into the terminal (end) vertex.
				boost::add_edge(non_terminal_vertex.first, terminal_vertex.first, 1, _cave_map);
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

			const auto cave_2 = _cave_map.insert_vertex(tunnel.second);
			_cave_map[tunnel.second] = tunnel.second;

			// We want to be able to go into and out of non-terminal caves.
			boost::add_edge(cave_1.first, cave_2.first, 1, _cave_map);
			boost::add_edge(cave_2.first, cave_1.first, 1, _cave_map);
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

class RouteIterator
{
	enum class RouteStatus
	{
		exploring,
		dead_end,
		found_end,
		invalid
	};

public:

	RouteIterator()
		: _caves{ nullptr }
	{}

	explicit RouteIterator(const CaveMap_t& caves)
		: _caves{ &caves }
	{
		_recurse_through_tunnels(Cave_t{ "start" });
	}

	RouteIterator(const RouteIterator&) = default;
	RouteIterator& operator=(const RouteIterator&) = default;
	RouteIterator(RouteIterator&&) = default;
	RouteIterator& operator=(RouteIterator&&) = default;

	bool operator==(const RouteIterator& other) const
	{
		return _current_route == other._current_route;
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
				continue;
			}
			case RouteStatus::found_end: {
				return *this;
			}
			default: {
				throw Exception("Unhandled result of exploring next tunnel");
			}
			}
		}

		return *this;
	}

	static bool is_singly_visitable(const Cave_t& cave)
	{
		if (cave == "start" || cave == "end") {
			return true;
		}

		return std::all_of(cave.begin(), cave.end(), [](auto c) { return std::islower(c); });
	}


private:
	using Breadcrumb_t = std::vector<std::pair<Cave_t, int>>;

	const CaveMap_t* _caves{ nullptr };
	std::set<Cave_t> _not_revisitable;
	Breadcrumb_t _current_route;

	
	RouteStatus _recurse_through_tunnels(const Cave_t& cave)
	{
		_current_route.emplace_back(cave, 0);

		if (!_caves) {
			return RouteStatus::invalid;
		}

		if (cave == "end") {
			return RouteStatus::found_end;
		}

		if (is_singly_visitable(cave)) {
			_not_revisitable.insert(cave);
		}

		auto status = RouteStatus::exploring;
		for (auto [edge, edges_end] = boost::out_edges(_caves->vertex(cave), *_caves);
			 edge != edges_end && status != RouteStatus::found_end;
			 ++edge) {

			const auto next_cave = _advance_into_cave(edge);

			if (_not_revisitable.contains(next_cave)) {
				continue;
			}

			status = _recurse_through_tunnels(next_cave);
		}

		if (status == RouteStatus::found_end) {
			return status;
		}
		else {
			// We explored all the tunnels from this cave and didn't find the end. This cave was en route to a dead-end; remove
			// from the stack and let the caller know.
			_pop_route();
			return RouteStatus::dead_end;
		}
	}

	void _pop_route()
	{
		if (_current_route.empty()) {
			return;
		}

		auto non_revisitable_cave = _not_revisitable.find(_current_route.back().first);
		if (_not_revisitable.end() != non_revisitable_cave) {
			_not_revisitable.erase(non_revisitable_cave);
		}

		_current_route.pop_back();
	}

	void _rewind_to_next_unexplored_tunnel()
	{
		_pop_route();

		if (_current_route.empty()) {
			return;
		}

		const auto [edges_begin, edges_end] = boost::out_edges(_caves->vertex(_current_route.back().first), *_caves);

		if (_current_route.back().second < std::distance(edges_begin, edges_end)) {
			return;
		}

		_rewind_to_next_unexplored_tunnel();
	}

	RouteStatus _explore_next_unexplored_tunnel()
	{
		if (!_caves) {
			return RouteStatus::invalid;
		}

		if (_current_route.empty()) {
			return RouteStatus::dead_end;
		}

		auto [edge, edges_end] = _find_next_unexplored_tunnel();
		if (edge == edges_end) {
			return RouteStatus::dead_end;
		}

		auto next_cave = _advance_into_cave(edge);

		while (_not_revisitable.contains(next_cave)) {
			if (++edge == edges_end) {
				_pop_route();
				return RouteStatus::dead_end;
			}

			next_cave = _advance_into_cave(edge);
		}

		return _recurse_through_tunnels(next_cave);
	}

	std::pair<CaveMap_t::out_edge_iterator, CaveMap_t::out_edge_iterator> _find_next_unexplored_tunnel()
	{
		auto [edge, edges_end] = boost::out_edges(_caves->vertex(_current_route.back().first), *_caves);

		while (_current_route.back().second >= std::distance(edge, edges_end)) {
			
			_pop_route();

			if (_current_route.empty()) {
				return { edges_end, edges_end };
			}

			std::tie(edge, edges_end) = boost::out_edges(_caves->vertex(_current_route.back().first), *_caves);
		}

		std::advance(edge, _current_route.back().second);

		return { edge, edges_end };
	}

	Cave_t _advance_into_cave(const CaveMap_t::out_edge_iterator& edge)
	{
		const auto out = _caves->graph()[boost::target(*edge, *_caves)];
		_current_route.back().second += 1;

		return out;
	}
};

///////////////////////////////////////////////////////////////////////////////

class CaveRoutes : public std::ranges::view_interface<CaveRoutes>
{
public:

	CaveRoutes(const CaveMap_t& caves)
		: _caves{ caves }
	{}

	auto begin() const { return RouteIterator{ _caves }; }
	auto begin() { return RouteIterator{_caves}; }
	auto end() const { return RouteIterator{}; }
	auto end() { return RouteIterator{}; }

private:
	const CaveMap_t& _caves;
};

///////////////////////////////////////////////////////////////////////////////

class CaveLoader
{
public:

	static CaveMap_t load(std::istream& is)
	{
		auto tunnels = _load_tunnels(is);

		const auto partitions = CaveMapBuilder::partition_tunnels(tunnels);

		auto caves = CaveMap_t{};
		CaveMapBuilder{ caves }
			.handle_terminal_cave<TerminalCaveType::start>(partitions.starts.begin(), partitions.starts.end())
			.handle_terminal_cave<TerminalCaveType::end>(partitions.ends.begin(), partitions.ends.end())
			.add_non_terminal_tunnels(partitions.others.begin(), partitions.others.end());

		return caves;
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
};

///////////////////////////////////////////////////////////////////////////////

class CaveRevisitor
{
public:
	CaveRevisitor(const CaveMap_t& caves)
		: _caves{ caves }
	{}

	std::vector<Route_t> routes() const
	{
		auto all_routes = std::vector<Route_t>{};
		std::copy(RouteIterator{ _caves }, RouteIterator{}, std::back_inserter(all_routes));

		for (const auto& cave : find_doubly_visitable_caves()) {
			auto routes_to_add = _find_routes_with_doubly_visitable_cave(cave);
			all_routes.insert(all_routes.end(), routes_to_add.begin(), routes_to_add.end());
		}

		return _make_routes_unique(std::move(all_routes));
	}

	std::vector<Cave_t> find_doubly_visitable_caves() const
	{
		auto [v, v_end] = boost::vertices(_caves);
		auto doubly_visitable_caves = std::ranges::subrange(v, v_end)
			| std::views::transform([this](auto vertex) { return _caves.graph()[vertex]; })
			| std::views::filter([this](const auto& cave) { return RouteIterator::is_singly_visitable(cave); })
			| std::views::filter([this](const auto& cave) {return cave != "start" && cave != "end"; });

		return std::vector<Cave_t>(doubly_visitable_caves.begin(), doubly_visitable_caves.end());
	}
private:

	std::vector<Route_t> _find_routes_with_doubly_visitable_cave(const Cave_t& cave) const
	{
		const auto amended_caves = _modify_caves_to_doubly_visit(cave);
		auto routes = std::vector<Route_t>{};
		std::copy(RouteIterator{ amended_caves }, RouteIterator{}, std::back_inserter(routes));
		return _convert_cave_names_to_standard_names(routes);
	}

	static std::vector<Route_t> _convert_cave_names_to_standard_names(std::vector<Route_t> routes)
	{
		std::transform(routes.begin(), routes.end(), routes.begin(), [](auto r) {
			std::transform(r.begin(), r.end(), r.begin(), [](auto cave) {
				return cave.erase(std::remove(cave.begin(), cave.end(), '_'), cave.end());
				});

			return r;
			});

		return routes;
	}

	CaveMap_t _modify_caves_to_doubly_visit(const Cave_t& doubly_visitable_cave) const
	{
		auto out = _caves;

		const auto cave_dual = Cave_t{ doubly_visitable_cave + "_" };
		auto dual_vertex = out.insert_vertex(cave_dual).first;

		for (auto [edge, edges_end] = boost::out_edges(out.vertex(doubly_visitable_cave), out); edge != edges_end; ++edge) {
			boost::add_edge(dual_vertex, boost::target(*edge, out), 1, out);
		}

		for (auto [edge, edges_end] = boost::in_edges(out.vertex(doubly_visitable_cave), out); edge != edges_end; ++edge) {
			boost::add_edge(boost::target(*edge, out), dual_vertex, 1, out);
		}

		return out;
	}

	static std::vector<Route_t> _make_routes_unique(std::vector<Route_t> routes)
	{
		std::ranges::sort(routes);
		routes.erase(std::unique(routes.begin(), routes.end()), routes.end());

		return routes;
	}

	const CaveMap_t& _caves;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
