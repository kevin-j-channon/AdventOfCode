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

	TEST_METHOD(DefaultConstructRouteIteratorThrowsIfDereferenced)
	{
		const auto it = aoc::RouteIterator{};
		Assert::ExpectException<aoc::OutOfRangeException>([&]() { *it; });
	}

	TEST_METHOD(RouteIteratorsCompareEqual)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{ {"a", "start"}, {"a", "end"} };

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin()))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin()), tunnels.end());

		const auto it_1 = aoc::RouteIterator{ cave_map };
		const auto it_2 = aoc::RouteIterator{ cave_map };

		Assert::IsTrue(it_1 == it_2);
	}

	TEST_METHOD(RouteIteratorsCompareNotEqual)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{ {"a", "start"}, {"start", "b"}, {"b", "end"}, {"a", "end"}};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin()))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin()), tunnels.end());

		auto it_1 = aoc::RouteIterator{ cave_map };
		auto it_2 = aoc::RouteIterator{ cave_map };

		Assert::IsTrue(++it_1 != it_2);
		Assert::IsTrue(it_1 == ++it_2);
	}

	TEST_METHOD(RouteIteratorsCompareEqualToSentinalIteratorAtEnd)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{ {"a", "start"}, {"a", "end"} };

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin()))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin()), tunnels.end());

		auto it_1 = aoc::RouteIterator{ cave_map };
		++it_1;	// There's only one route, so now this should be at "end"

		Assert::IsTrue(it_1 == aoc::RouteIterator{});
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

		const auto cave_map = aoc::CaveLoader{}.load(data);

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "A" }, cave_map["A"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "c" }, cave_map["c"]);
		Assert::AreEqual(aoc::Cave_t{ "d" }, cave_map["d"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 10 }, boost::num_edges(cave_map.graph()));
	}

	
	TEST_METHOD(FindAllRoutesWorksForTrivialCaveSystem)
	{
		constexpr auto data_str =
			"start-a\n"
			"start-b\n"
			"a-end\n"
			"b-end"
			;

		std::stringstream data(data_str);
		auto caves = aoc::CaveLoader{}.load(data);
		auto routes = aoc::CaveRoutes{ caves };

		const auto expected_routes = { "start,a,end"s, "start,b,end"s };
		auto expected = expected_routes.begin();
		for (auto route = routes.begin(); route != routes.end(); ++route, ++expected) {
			Assert::AreEqual(*expected, aoc::route::as_string(*route));
		}
	}

	TEST_METHOD(FindAllCavesForExample)
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
		auto caves = aoc::CaveLoader::load(data);
		auto routes = aoc::CaveRoutes{ caves };

		const auto expected_routes = std::array<std::string, 10>{
			"start,A,end",
			"start,A,b,end",
			"start,A,b,A,end",
			"start,A,b,A,c,A,end",
			"start,A,c,A,end",
			"start,A,c,A,b,end",
			"start,A,c,A,b,A,end",
			"start,b,end",
			"start,b,A,end",
			"start,b,A,c,A,end"
		};

		auto expected = expected_routes.begin();
		for (auto route = routes.begin(); route != routes.end(); ++expected, ++route) {
			Assert::AreEqual(*expected, aoc::route::as_string(*route));
		}
	}
	TEST_METHOD(FindAllCavesForLargeExample)
	{
		constexpr auto data_str =
			"dc-end\n"
			"HN-start\n"
			"start-kj\n"
			"dc-start\n"
			"dc-HN\n"
			"LN-dc\n"
			"HN-end\n"
			"kj-sa\n"
			"kj-HN\n"
			"kj-dc"
			;

		std::stringstream data(data_str);
		auto caves = aoc::CaveLoader::load(data);
		auto routes = aoc::CaveRoutes{ caves };
		auto route_count = std::accumulate(routes.begin(), routes.end(), uint32_t{ 0 }, [](auto curr, auto _) { return ++curr; });

		Assert::AreEqual(uint32_t{ 19 }, route_count);
	}
	TEST_METHOD(FindAllCavesForLargerExample)
	{
		constexpr auto data_str =
			"fs-end\n"
			"he-DX\n"
			"fs-he\n"
			"start-DX\n"
			"pj-DX\n"
			"end-zg\n"
			"zg-sl\n"
			"zg-pj\n"
			"pj-he\n"
			"RW-he\n"
			"fs-DX\n"
			"pj-RW\n"
			"zg-RW\n"
			"start-pj\n"
			"he-WI\n"
			"zg-he\n"
			"pj-fs\n"
			"start-RW"
			;

		std::stringstream data(data_str);
		auto caves = aoc::CaveLoader::load(data);
		auto routes = aoc::CaveRoutes{ caves };
		auto route_count = std::accumulate(routes.begin(), routes.end(), uint32_t{ 0 }, [](auto curr, auto _) { return ++curr; });

		Assert::AreEqual(uint32_t{ 226 }, route_count);
	}
};

TEST_CLASS(TestCaveRevisitor)
{
public:

	TEST_METHOD(DoublyVisitableCavesAreFound)
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

		auto caves = aoc::CaveMap_t{};

		aoc::CaveMapBuilder{ caves }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), 5), tunnels.end())
			.add_non_terminal_tunnels(std::next(tunnels.begin(), 2), std::next(tunnels.begin(), 5));

		auto doubly_visitable_caves = aoc::CaveRevisitor{ caves }.find_doubly_visitable_caves();
		std::ranges::sort(doubly_visitable_caves);
		Assert::AreEqual(size_t{ 3 }, doubly_visitable_caves.size());
		
		auto expected_caves = { "b", "c", "d" };
		Assert::IsTrue(std::ranges::equal(expected_caves, doubly_visitable_caves));
	}

	TEST_METHOD(WorksForSimplestExample)
	{
		auto tunnels = std::vector<aoc::Tunnel_t>{
				{"start", "A"},
				{"A", "b"},
				{"A", "end"}
		};

		auto caves = aoc::CaveMap_t{};

		aoc::CaveMapBuilder{ caves }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin()))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), 2), tunnels.end())
			.add_non_terminal_tunnels(std::next(tunnels.begin()), std::next(tunnels.begin(), 2));

		auto routes = aoc::CaveRevisitor{ caves }.routes();

		const auto expected_routes = {
			"start,A,b,A,b,A,end"s,
			"start,A,b,A,end"s,
			"start,A,end"s
		};

		Assert::AreEqual(expected_routes.size(), routes.size());

		auto expected = expected_routes.begin();
		for (auto route = routes.begin(); route != routes.end(); ++route, ++expected) {
			Assert::AreEqual(*expected, aoc::route::as_string(*route));
		}
	}
};
}
