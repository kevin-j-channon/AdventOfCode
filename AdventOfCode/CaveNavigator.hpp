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

///////////////////////////////////////////////////////////////////////////////

class CaveNavigator
{
public:

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

	const CaveMap_t& cave_map() const { return _cave_system; }

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
