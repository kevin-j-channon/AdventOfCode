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
		start,
		end
	};

	template<size_t type_T>
	static constexpr const char* type() {
		static_assert(false, "Invalid terminal cave type");
	}

	template<>
	static constexpr const char* type<start>() { return "start"; }

	template<>
	static constexpr const char* type<end>() { return "end"; }
};

class CaveMapBuilder
{

	CaveMap_t& _cave_map;
public:

	CaveMapBuilder(CaveMap_t& cave_map)
		: _cave_map{cave_map}
	{}

	template<size_t TYPE, typename TunnelIter_T>
	CaveMapBuilder& handle_terminal_cave(TunnelIter_T begin, TunnelIter_T end)
	{
		const auto terminal_vertex = _cave_map.insert_vertex(TerminalCaveType::type<TYPE>());
		_cave_map[TerminalCaveType::type<TYPE>()] = TerminalCaveType::type<TYPE>();

		std::for_each(begin, end, [&](const auto& tunnel) {
			const auto name = tunnel.first == TerminalCaveType::type<TYPE>() ? tunnel.second : tunnel.first;
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
