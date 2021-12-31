#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "CaveNavigator.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace test_cave_nav
{
TEST_CLASS(TestCaveBuilder)
{
public:

	TEST_METHOD(CanBuildTunnelsTrivial_1)
	{
		auto cave_map = aoc::CaveMap_t{};
		auto builder = aoc::CaveMapBuilder{ cave_map };

		auto tunnels = std::vector<aoc::Tunnel_t>{ {"start", "end"} };

		builder.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin()));

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 1 }, boost::num_edges(cave_map.graph()));
	}

	TEST_METHOD(CanBuildTunnelsTrivial_2)
	{
		auto cave_map = aoc::CaveMap_t{};
		auto builder = aoc::CaveMapBuilder{ cave_map };

		auto tunnels = std::vector<aoc::Tunnel_t>{ {"end", "start"} };

		builder.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin()));

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 1 }, boost::num_edges(cave_map.graph()));
	}

	TEST_METHOD(CanBuildTunnelsSingleIntermediate)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{ {"a", "start"}, {"a", "end"} };

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin()))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin()), tunnels.end());

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "a" }, cave_map["a"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 2 }, boost::num_edges(cave_map.graph()));
	}


	TEST_METHOD(CanBuildTunnelsTwoIntermediates)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"a", "start"},
			{"start", "b"},
			{"a", "end"},
			{"b", "end"}
		};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), tunnels.size() / 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), tunnels.size() / 2), tunnels.end());

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "a" }, cave_map["a"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 4 }, boost::num_edges(cave_map.graph()));
	}


	TEST_METHOD(CanBuildTunnelsFiveIntermediates)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"a", "start"},
			{"start", "b"},
			{"start", "c"},
			{"start", "d"},
			{"start", "e"},
			{"a", "end"},
			{"b", "end"},
			{"c", "end"},
			{"d", "end"},
			{"e", "end"}
		};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), tunnels.size() / 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), tunnels.size() / 2), tunnels.end());

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "a" }, cave_map["a"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "c" }, cave_map["c"]);
		Assert::AreEqual(aoc::Cave_t{ "d" }, cave_map["d"]);
		Assert::AreEqual(aoc::Cave_t{ "e" }, cave_map["e"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 10 }, boost::num_edges(cave_map.graph()));
	}


	TEST_METHOD(CanBuildTunnelsTwoConnectedIntermediates)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"a", "start"},
			{"start", "b"},
			{"a", "b"},
			{"a", "end"},
			{"b", "end"}
		};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), 3), tunnels.end())
			.add_non_terminal_tunnels(std::next(tunnels.begin(), 2), std::next(tunnels.begin(), 3));

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "a" }, cave_map["a"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 6 }, boost::num_edges(cave_map.graph()));
	}

	TEST_METHOD(CanBuildExample)
	{
		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"start", "A"},
			{"start", "b"},
			{"A", "c"},
			{"A", "b"},
			{"b", "d"},
			{"A", "end"},
			{"b", "end"}
		};

		auto cave_map = aoc::CaveMap_t{};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), 5), tunnels.end())
			.add_non_terminal_tunnels(std::next(tunnels.begin(), 2), std::next(tunnels.begin(), 5));

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "A" }, cave_map["A"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "c" }, cave_map["c"]);
		Assert::AreEqual(aoc::Cave_t{ "d" }, cave_map["d"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 10 }, boost::num_edges(cave_map.graph()));
	}

	TEST_METHOD(TerminalCaveTypeIsCorrect)
	{
		Assert::IsTrue(aoc::TerminalCaveType::start == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "start", "a" }));
		Assert::IsTrue(aoc::TerminalCaveType::start == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "a", "start" }));

		Assert::IsTrue(aoc::TerminalCaveType::end == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "end", "a" }));
		Assert::IsTrue(aoc::TerminalCaveType::end == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "a", "end" }));

		Assert::IsTrue((aoc::TerminalCaveType::end | aoc::TerminalCaveType::start) == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "start", "end" }));
		Assert::IsTrue((aoc::TerminalCaveType::end | aoc::TerminalCaveType::start) == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "end", "start" }));

		Assert::IsTrue(aoc::TerminalCaveType::none == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "b", "a" }));
		Assert::IsTrue(aoc::TerminalCaveType::none == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "a", "b" }));
	}

	TEST_METHOD(PartitioningTunnelsWorks)
	{
		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"pf", "pk"},
			{"ZQ", "iz"},
			{"iz", "NY"},
			{"ZQ", "end"},
			{"pf", "gx"},
			{"pk", "ZQ"},
			{"ZQ", "dc"},
			{"NY", "start"},
			{"NY", "pf"},
			{"NY", "gx"},
			{"ag", "ZQ"},
			{"pf", "start"},
			{"start", "gx"},
			{"BN", "ag"},
			{"iz", "pf"},
			{"ag", "FD"},
			{"pk", "NY"},
			{"gx", "pk"},
			{"end", "BN"},
			{"ag", "pf"},
			{"iz", "pk"},
			{"pk", "ag"},
			{"iz", "end"},
			{"iz", "BN"}
		};

		const auto partitions = aoc::CaveMapBuilder::partition_tunnels(tunnels);

		Assert::IsTrue(tunnels.begin() == partitions.starts.begin());
		Assert::IsTrue(std::next(tunnels.begin(), 3) == partitions.starts.end());

		for (const auto& t : partitions.starts) {
			Assert::IsTrue(aoc::TerminalCaveType::start == aoc::TerminalCaveType::to_type(t));
		}

		Assert::IsTrue(std::next(tunnels.begin(), 3) == partitions.ends.begin());
		Assert::IsTrue(std::next(tunnels.begin(), 6) == partitions.ends.end());

		for (const auto& t : partitions.ends) {
			Assert::IsTrue(aoc::TerminalCaveType::end == aoc::TerminalCaveType::to_type(t));
		}

		Assert::IsTrue(std::next(tunnels.begin(), 6) == partitions.others.begin());
		Assert::IsTrue(tunnels.end() == partitions.others.end());

		for (const auto& t : partitions.others) {
			Assert::IsTrue(aoc::TerminalCaveType::none == aoc::TerminalCaveType::to_type(t));
		}
	}
};

TEST_CLASS(TestRouteIterator)
{
public:
	TEST_METHOD(FirstRouteIsFoundOnConstruction)
	{
		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"start", "A"},
			{"start", "b"},
			{"A", "c"},
			{"A", "b"},
			{"b", "d"},
			{"A", "end"},
			{"b", "end"}
		};

		auto cave_map = aoc::CaveMap_t{};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), 5), tunnels.end())
			.add_non_terminal_tunnels(std::next(tunnels.begin(), 2), std::next(tunnels.begin(), 5));

		const auto routes = aoc::RouteIterator{ cave_map };

		Assert::AreEqual("start,A,end"s, aoc::route::as_string(*routes));
	}

	TEST_METHOD(IncrementRouteIterator)
	{
		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"start", "A"},
			{"start", "b"},
			{"A", "c"},
			{"A", "b"},
			{"b", "d"},
			{"A", "end"},
			{"b", "end"}
		};

		auto cave_map = aoc::CaveMap_t{};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), 5), tunnels.end())
			.add_non_terminal_tunnels(std::next(tunnels.begin(), 2), std::next(tunnels.begin(), 5));

		auto routes = aoc::RouteIterator{ cave_map };

		Assert::AreEqual("start,A,c,A,end"s, aoc::route::as_string(*(++routes)));
	}

	TEST_METHOD(SamplePathsCanAllBeIterated)
	{
		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"start", "A"},
			{"start", "b"},
			{"A", "c"},
			{"A", "b"},
			{"b", "d"},
			{"A", "end"},
			{"b", "end"}
		};

		auto cave_map = aoc::CaveMap_t{};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), 5), tunnels.end())
			.add_non_terminal_tunnels(std::next(tunnels.begin(), 2), std::next(tunnels.begin(), 5));

		auto route = aoc::RouteIterator{ cave_map };

		const auto expected_routes = std::array<std::string, 10>{
			"start,A,end",
			"start,A,c,A,end",
			"start,A,c,A,b,end",
			"start,A,c,A,b,A,end",
			"start,A,b,end",
			"start,A,b,A,end",
			"start,A,b,A,c,A,end",
			"start,b,end",
			"start,b,A,end",
			"start,b,A,c,A,end",

		};

		for (auto expected = expected_routes.begin(); expected != expected_routes.end(); ++expected, ++route) {
			Assert::AreEqual(*expected, aoc::route::as_string(*route));
		}
	}
};

TEST_CLASS(TestCaveNavigator)
{
public:

	TEST_METHOD(CavesCanBeLoadedFromStream)
	{
		constexpr auto data_str =
			"start-A\n"
			"start-b\n"
			"A-c\n"
			"A-b\n"
			"b-d\n"
			"A-end\n"
			"b-end"
			;

		std::stringstream data(data_str);

		const auto cave_map = aoc::CaveNavigator{}.load(data).cave_map();

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "A" }, cave_map["A"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "c" }, cave_map["c"]);
		Assert::AreEqual(aoc::Cave_t{ "d" }, cave_map["d"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 10 }, boost::num_edges(cave_map.graph()));
	}

	/*
	TEST_METHOD(FindAllPathsWorksForTrivialCaveSystem)
	{
		constexpr auto data_str =
			"start-a\n"
			"start-b\n"
			"a-end\n"
			"b-end"
			;

		std::stringstream data(data_str);

		const auto paths = aoc::CaveNavigator{}.load(data).paths();

		const auto expected = { "start,a,end"s, "start,b,end"s };

		Assert::AreEqual(__int64(expected.size()), paths.size());
		Assert::IsTrue(std::equal(expected.begin(), expected.end(), paths.begin()));
	}
	*/
};
}