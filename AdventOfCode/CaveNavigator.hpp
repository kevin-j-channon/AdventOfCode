#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>

#include <istream>
#include <string>
#include <utility>
#include <algorithm>
#include <vector>
#include <ranges>

namespace aoc
{

using Cave_t = std::string;
using Tunnel_t = std::pair<Cave_t, Cave_t>;

using EdgeWeightProperty_t = boost::property<boost::edge_weight_t, int>;
using CaveMap_t = boost::labeled_graph<boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Cave_t, EdgeWeightProperty_t>, std::string>;

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
		_cave_map[TerminalCaveType::name<TYPE>()] = TerminalCaveType::name<TYPE>();

		std::for_each(begin, end, [&](const auto& tunnel) {
			const auto name = tunnel.first == TerminalCaveType::name<TYPE>() ? tunnel.second : tunnel.first;
			const auto non_terminal_vertex = _cave_map.insert_vertex(name);
			_cave_map[name] = name;

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
			boost::add_edge(cave_2.first, cave_2.first, 1, _cave_map);
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

class CaveNavigator
{
public:

	
	struct StartAndEndCaves {
		const std::string* start{ nullptr };
		const std::string* end{ nullptr };
		const std::string* other_cave{ nullptr };
	};

	CaveNavigator& load(std::istream& is)
	{
		while (!is.eof()) {
			_load_tunnel(is);
		}

		return *this;
	}

	static StartAndEndCaves get_start_or_end_cave(const Tunnel_t& t)
	{
		auto out = StartAndEndCaves{};

		if (t.first == "start") {
			out.start = &t.first;
		}
		if (t.first == "end") {
			out.end = &t.first;
		}
		if (!(out.start || out.end)) {
			// We didn't find the first cave to be start, or end, so it's "other".
			out.other_cave = &t.first;
		}

		if (!out.start && t.second == "start") {
			out.start = &t.second;
			if (!out.end) {
				// The second cave was start, but the first cave wasn't the "end" cave, so it must be "other".
				out.other_cave = &t.first;
			}
		}
		if (!out.end && t.second == "end") {
			out.end = &t.second;
			if (!out.start) {
				// The second cave was end, but the first cave wasn't the "start" cave, so it must be "other".
				out.other_cave = &t.first;
			}
		}

		return out;
	}

private:

	static std::string _get_line_from_stream(std::istream& is)
	{
		auto line = std::string{};
		std::getline(is, line);
		if (is.fail()) {
			throw Exception("Failed to read edge from file");
		}

		return line;
	}

	void _load_tunnel(std::istream& is)
	{
		auto line = _get_line_from_stream(is);
		auto tunnel = _get_tunnel_from_string(std::move(line));
		_add_tunnel(std::move(tunnel));
	}

	static Tunnel_t _get_tunnel_from_string(std::string s)
	{
		const auto caves = split(s, '-');
		if (caves.size() != 2) {
			throw Exception(std::format("Invalid tunnel line: {}", s));
		}

		return { caves[0], caves[1] };
	}

	void _add_tunnel(Tunnel_t t)
	{
		_add_caves_from_tunnel(t);

		boost::add_edge_by_label(t.first, t.second, _cave_system);

	}

	void _add_caves_from_tunnel(const Tunnel_t& t)
	{
		_cave_system.insert_vertex(t.first);
		_cave_system.insert_vertex(t.second);
	}

	CaveMap_t _cave_system;
};
}
