#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "Polymerizer.hpp"

using namespace std::string_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace test_polymerizer
{
TEST_CLASS(TestPolymer)
{
public:
	TEST_METHOD(LoadStreamFromStream)
	{
		std::stringstream data("NNCB");

		const auto polymer = aoc::polymer::Polymer::from_stream(data);

		Assert::AreEqual(size_t{ 4 }, polymer.length());
		Assert::AreEqual("NNCB"s, polymer.as_string());
	}

	TEST_METHOD(SamplePolymerization_1)
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
		const auto rules = aoc::polymer::InsertionRuleLoader::insertion_rule_table_from_stream(data);

		polymer.polymerize<1>(rules);

		Assert::AreEqual(size_t{7}, polymer.length());
		Assert::AreEqual("NCNBCHB"s, polymer.as_string());
	}

	TEST_METHOD(SamplePolymerization_2)
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
		const auto rules = aoc::polymer::InsertionRuleLoader::insertion_rule_table_from_stream(data);

		polymer.polymerize<2>(rules);

		Assert::AreEqual(size_t{ 13 }, polymer.length());
		Assert::AreEqual("NBCCNBBBCBHCB"s, polymer.as_string());
	}

	TEST_METHOD(SamplePolymerization_3)
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
		const auto rules = aoc::polymer::InsertionRuleLoader::insertion_rule_table_from_stream(data);

		polymer.polymerize<3>(rules);

		Assert::AreEqual(size_t{ 25 }, polymer.length());
		Assert::AreEqual("NBBBCNCCNBBNBNBBCHBHHBCHB"s, polymer.as_string());
	}

	TEST_METHOD(SamplePolymerization_4)
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
		const auto rules = aoc::polymer::InsertionRuleLoader::insertion_rule_table_from_stream(data);

		polymer.polymerize<4>(rules);

		Assert::AreEqual(size_t{ 49 }, polymer.length());
		Assert::AreEqual("NBBNBNBBCCNBCNCCNBBNBBNBBBNBBNBBCBHCBHHNHCBBCBHCB"s, polymer.as_string());
	}
};

TEST_CLASS(TestRuleLoader)
{
public:
	TEST_METHOD(LoadRuleFromStream)
	{
		std::stringstream data("AB -> C");
		const auto rules = aoc::polymer::InsertionRuleLoader::insertion_rule_table_from_stream(data);

		Assert::AreEqual(size_t{ 1 }, rules.size());
		Assert::AreEqual('C', rules.at(aoc::polymer::Dimer_t{ 'A', 'B' }));
	}

	TEST_METHOD(LoadMultipleRulesFromStream)
	{
		std::stringstream data("AB -> C\nDE -> F\nGH -> I\nJK -> L");
		const auto rules = aoc::polymer::InsertionRuleLoader::insertion_rule_table_from_stream(data);

		Assert::AreEqual(size_t{ 4 }, rules.size());
		Assert::AreEqual('C', rules.at(aoc::polymer::Dimer_t{ 'A', 'B' }));
		Assert::AreEqual('F', rules.at(aoc::polymer::Dimer_t{ 'D', 'E' }));
		Assert::AreEqual('I', rules.at(aoc::polymer::Dimer_t{ 'G', 'H' }));
		Assert::AreEqual('L', rules.at(aoc::polymer::Dimer_t{ 'J', 'K' }));
	}
};

TEST_CLASS(TestScorer)
{
public:
	TEST_METHOD(CalculateWorksForExample)
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
		const auto rules = aoc::polymer::InsertionRuleLoader::insertion_rule_table_from_stream(data);

		polymer.polymerize<10>(rules);
		Assert::AreEqual(size_t{ 3073 }, polymer.length());

		const auto score = aoc::polymer::Scorer{ polymer }.calculate();
		Assert::AreEqual(uint32_t{ 1588 }, score);
	}
};
}
