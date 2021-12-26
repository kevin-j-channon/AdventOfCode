#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>

#include <istream>
#include <string>
#include <utility>

namespace aoc
{
class CaveNavigator
{
public:

	using Cave_t = std::string;
	using Tunnel_t = std::pair<Cave_t, Cave_t>;

	using Graph_t = boost::labeled_graph<boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Cave_t, boost::no_property>, std::string>;
	using EdgeIter_t = boost::graph_traits<Graph_t>::edge_iterator;

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

	Graph_t _cave_system;
};
}
