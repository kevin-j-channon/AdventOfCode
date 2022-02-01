#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "SnailfishNumbers.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<aoc::snailfish::Value>(const aoc::snailfish::Value& value)
{
	return value.as_string<wchar_t>();
}

namespace test_snailfish_numbers
{

using namespace aoc::snailfish;

TEST_CLASS(TestInitialisation)
{
public:
	TEST_METHOD(FromStreamSucceedsForSimplestGoodStream)
	{
		std::stringstream data{ "[1,2]" };
		const auto n1 = aoc::snailfish::Value::from_stream(data);
		const auto n2 = aoc::snailfish::Value{ 1, 2 };

		Assert::AreEqual(n2, n1);
	}

	TEST_METHOD(FailsIfStreamIsEmpty)
	{
		std::stringstream data{ "" };
		Assert::ExpectException<aoc::IOException>([&data]() {aoc::snailfish::Value::from_stream(data); });
	}

	TEST_METHOD(FailsIfStreamDoesntStartWithSquareBracket)
	{
		std::stringstream data{ "{1,2]" };
		Assert::ExpectException<aoc::IOException>([&data]() {aoc::snailfish::Value::from_stream(data); });
	}

	TEST_METHOD(FailsIfStreamDoesntEndWithSquareBracket)
	{
		std::stringstream data{ "[1,2" };
		Assert::ExpectException<aoc::IOException>([&data]() {aoc::snailfish::Value::from_stream(data); });
	}

	TEST_METHOD(FailsIfThereAreNotTwoValues)
	{
		{
			std::stringstream data{ "[1]" };
			Assert::ExpectException<aoc::IOException>([&data]() {aoc::snailfish::Value::from_stream(data); });
		}

		{
			std::stringstream data{ "[1,2,3]" };
			Assert::ExpectException<aoc::IOException>([&data]() {aoc::snailfish::Value::from_stream(data); });
		}
	}

	TEST_METHOD(FromStreamSucceedsForNestedValue_1)
	{
		const auto data_str = "[1,[2,3]]"s;

		std::stringstream data{ data_str };
		const auto n = aoc::snailfish::Value::from_stream(data);

		Assert::AreEqual(data_str, n.as_string<char>());
	}

	TEST_METHOD(FromStreamSucceedsForNestedValue_2)
	{
		const auto data_str = "[[1,2],3]"s;

		std::stringstream data{ data_str };
		const auto n = aoc::snailfish::Value::from_stream(data);

		Assert::AreEqual(data_str, n.as_string<char>()		);
	}

	TEST_METHOD(FromStreamSucceedsForNestedValue_3)
	{
		const auto data_str = "[[1,2],[3,4]]"s;

		std::stringstream data{ data_str };
		const auto n = aoc::snailfish::Value::from_stream(data);

		Assert::AreEqual(data_str, n.as_string<char>());
	}

	TEST_METHOD(FromStreamSucceedsForNestedValue_4)
	{
		const auto data_str = "[[1,[2,5]],[3,4]]"s;

		std::stringstream data{ data_str };
		const auto n = aoc::snailfish::Value::from_stream(data);

		Assert::AreEqual(data_str, n.as_string<char>());
	}

	TEST_METHOD(SampleValues)
	{
		const auto data_strs = std::array{
			"[[[[[9,8],1],2],3],4]"s,
			"[7,[6,[5,[4,[3,2]]]]]"s,
			"[[6,[5,[4,[3,2]]]],1]"s,
			"[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]"s,
			"[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]"s,
			"[[[[1,1],[2,2]],[3,3]],[4,4]]"s
		};

		for (const auto& data_str : data_strs) {
			std::stringstream data{ data_str };
			const auto n = aoc::snailfish::Value::from_stream(data);

			Assert::AreEqual(data_str, n.as_string<char>());
		}
	}

	TEST_METHOD(CopyConstructSimple)
	{
		const Value v1{ 1, 3 };
		const auto v2{ v1 };

		Assert::AreEqual(v1, v2);
	}

	TEST_METHOD(CopyConstructSamples)
	{
		const auto data_strs = std::array{
			"[[[[[9,8],1],2],3],4]"s,
			"[7,[6,[5,[4,[3,2]]]]]"s,
			"[[6,[5,[4,[3,2]]]],1]"s,
			"[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]"s,
			"[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]"s,
			"[[[[1,1],[2,2]],[3,3]],[4,4]]"s
		};

		for (const auto& data_str : data_strs) {
			std::stringstream data{ data_str };
			const auto v1 = aoc::snailfish::Value::from_stream(data);
			const auto v2{ v1 };

			Assert::AreEqual(v1, v2);
		}
	}

	TEST_METHOD(AssignmentOperatorSimple)
	{
		const Value v1{ 1, 3 };
		const auto v2 = v1;

		Assert::AreEqual(v1, v2);
	}

	TEST_METHOD(AssignmentOperatorSamples)
	{
		const auto data_strs = std::array{
			"[[[[[9,8],1],2],3],4]"s,
			"[7,[6,[5,[4,[3,2]]]]]"s,
			"[[6,[5,[4,[3,2]]]],1]"s,
			"[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]"s,
			"[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]"s,
			"[[[[1,1],[2,2]],[3,3]],[4,4]]"s
		};

		for (const auto& data_str : data_strs) {
			std::stringstream data{ data_str };
			const auto v1 = aoc::snailfish::Value::from_stream(data);
			const auto v2 = v1;

			Assert::AreEqual(v1, v2);
		}
	}
};

TEST_CLASS(TestAlgebra)
{
public:
	/*
	TEST_METHOD(AddTwoValues)
	{
		const auto sum = Value{ 1, 2 } + Value{ 3, 4 };
		Assert::AreEqual("[[1,2],[3,4]]", sum.as_string<char>());
	}
	*/
};
}