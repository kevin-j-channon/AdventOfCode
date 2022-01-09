#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "Polymerizer.hpp"

using namespace std::string_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace test_polymerizer
{

TEST_CLASS(TestRuleLoader)
{
public:
	TEST_METHOD(LoadRuleFromStream)
	{
		std::stringstream data("AB -> C");
		const auto rules = aoc::polymer::InsertionRuleLoader::from_stream(data);

		Assert::AreEqual(size_t{ 1 }, rules.size());
		Assert::AreEqual('C', rules.at(aoc::polymer::Dimer_t{ 'A', 'B' }));
	}

	TEST_METHOD(LoadMultipleRulesFromStream)
	{
		std::stringstream data("AB -> C\nDE -> F\nGH -> I\nJK -> L");
		const auto rules = aoc::polymer::InsertionRuleLoader::from_stream(data);

		Assert::AreEqual(size_t{ 4 }, rules.size());
		Assert::AreEqual('C', rules.at(aoc::polymer::Dimer_t{ 'A', 'B' }));
		Assert::AreEqual('F', rules.at(aoc::polymer::Dimer_t{ 'D', 'E' }));
		Assert::AreEqual('I', rules.at(aoc::polymer::Dimer_t{ 'G', 'H' }));
		Assert::AreEqual('L', rules.at(aoc::polymer::Dimer_t{ 'J', 'K' }));
	}
};

TEST_CLASS(TestPolymer)
{
public:
	TEST_METHOD(SimplePolymerScoresCorrectly)
	{
		std::stringstream data("AA -> B");
		const auto rules = aoc::polymer::InsertionRuleLoader::from_stream(data);
		
		const auto polymer = std::move(aoc::polymer::Polymer{ "AA"s }.polymerize(1, rules));

		const auto abundances = polymer.abundances();
		Assert::AreEqual(uint64_t{ 2 }, abundances.at('A'));
		Assert::AreEqual(uint64_t{ 1 }, abundances.at('B'));

		const auto score = polymer.score();
		Assert::AreEqual(uint64_t{ 1 }, score);
	}

	TEST_METHOD(ExampleScoresCorrectly_10Cycles)
	{
		constexpr auto data_str =
			"NNCB\n"
			"\n"
			"HH -> N\n"
			"CB -> H\n"
			"NH -> C\n"
			"CH -> B\n"
			"HB -> C\n"
			"HC -> B\n"
			"HN -> C\n"
			"NN -> C\n"
			"BH -> H\n"
			"NC -> B\n"
			"NB -> B\n"
			"BN -> B\n"
			"BB -> N\n"
			"BC -> B\n"
			"CC -> N\n"
			"CN -> C"
			;

		std::stringstream data(data_str);

		auto polymer = aoc::polymer::Polymer::from_stream(data);
		const auto rules = aoc::polymer::InsertionRuleLoader::from_stream(data);

		const auto score = polymer.polymerize(10, rules).score();
		Assert::AreEqual(uint64_t{ 1588 }, score);
	}

	TEST_METHOD(ExampleScoresCorrectly_40Cycles)
	{
		constexpr auto data_str =
			"NNCB\n"
			"\n"
			"HH -> N\n"
			"CB -> H\n"
			"NH -> C\n"
			"CH -> B\n"
			"HB -> C\n"
			"HC -> B\n"
			"HN -> C\n"
			"NN -> C\n"
			"BH -> H\n"
			"NC -> B\n"
			"NB -> B\n"
			"BN -> B\n"
			"BB -> N\n"
			"BC -> B\n"
			"CC -> N\n"
			"CN -> C"
			;

		std::stringstream data(data_str);

		auto polymer = aoc::polymer::Polymer::from_stream(data);
		const auto rules = aoc::polymer::InsertionRuleLoader::from_stream(data);

		const auto score = polymer.polymerize(40, rules).score();
		Assert::AreEqual(uint64_t{ 2188189693529 }, score);
	}
};
}
