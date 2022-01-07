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
}
