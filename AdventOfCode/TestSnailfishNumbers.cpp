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

TEST_CLASS(TestParentsAndChildren)
{
public:
	TEST_METHOD(ChildrenAreCorrect)
	{
		const auto value = Value::from_string("[[1,2],[3,4]]");
		Assert::AreEqual("[1,2]"s, value.child<ChildPosition::left>().as_string<char>());
		Assert::AreEqual("[3,4]"s, value.child<ChildPosition::right>().as_string<char>());
	}

	TEST_METHOD(NestedChildIsCorrect)
	{
		const auto value = Value::from_string("[[1,[5,6]],[3,4]]");
		const auto child_str = value
			.child<ChildPosition::left>()
			.as<ValuePtr_t>()
			->child<ChildPosition::right>()
			.as_string<char>();

		Assert::AreEqual("[5,6]"s, child_str);
	}

	TEST_METHOD(ParentIsNulByDefault)
	{
		const auto value = Value::from_string("[8,9]");
		Assert::IsNull(value.parent());
	}

	TEST_METHOD(ParentIsCorrect)
	{
		const auto value = Value::from_string("[[1,[5,6]],[3,4]]");
		const auto& grand_child = value
			.child<ChildPosition::left>()
			.as<Value>()
			.child<ChildPosition::right>()
			.as<Value>();

		Logger::WriteMessage(std::format("Child value: {}\n", value.child<ChildPosition::left>().as<Value>().as_string<char>()).c_str());
		Logger::WriteMessage(std::format("Child's parent: {}\n", value.child<ChildPosition::left>().as<Value>().parent()->as_string<char>()).c_str());

		Logger::WriteMessage(std::format("Grandchild value: {}\n", value.child<ChildPosition::left>().as<Value>().child<ChildPosition::right>().as<Value>().as_string<char>()).c_str());
		Logger::WriteMessage(std::format("Grandchild's parent: {}\n", value.child<ChildPosition::left>().as<Value>().child<ChildPosition::right>().as<Value>().parent()->parent()->as_string<char>()).c_str());

		auto* p = grand_child.parent();
		auto* gp = p->parent();

		Assert::AreEqual("[1,[5,6]]"s, grand_child.parent()->as_string<char>());
		Assert::AreEqual(value.as_string<char>(), grand_child.parent()->parent()->as_string<char>());
	}
};

TEST_CLASS(TestAccessors)
{
public:
	TEST_METHOD(AsValuePtr)
	{
		const auto value = Value::from_string("[[1,[5,6]],[3,4]]");
		const auto child = value.child<ChildPosition::left>().as<ValuePtr_t>();

		Assert::AreEqual("[1,[5,6]]"s, child->as_string<char>());
	}

	TEST_METHOD(AsValueRef)
	{
		const auto value = Value::from_string("[[1,[5,6]],[3,4]]");
		const auto& child = value.child<ChildPosition::left>().as<Value>();

		Assert::AreEqual("[1,[5,6]]"s, child.as_string<char>());
	}

	TEST_METHOD(AsInt)
	{
		const auto value = Value::from_string("[[1,[5,6]],[3,4]]");
		const auto child = value.child<ChildPosition::left>().as<Value>().child<ChildPosition::left>().as<uint32_t>();

		Assert::AreEqual(uint32_t{ 1 }, child);
	}
};

TEST_CLASS(TestAlgebra)
{
public:
	
	TEST_METHOD(CompoundAssignmentIncrementSimple)
	{
		auto sum = Value{ 1, 2 };
		sum += Value{ 3, 4 };

		Assert::AreEqual("[[1,2],[3,4]]"s, sum.as_string<char>());
	}

	TEST_METHOD(CompoundAssignmentIncrementComplex)
	{
		auto sum = Value::from_string("[[1,2],[[3,4],[5,6]]]"s);
		sum += Value::from_string("[1,[2,[[3,4],5]]]"s);

		Assert::AreEqual("[[[1,2],[[3,4],[5,6]]],[1,[5,[0,9]]]]"s, sum.as_string<char>());
	}

	TEST_METHOD(AdditionSimple)
	{
		const auto sum = Value{ 1, 2 } + Value{ 3, 4 };
		Assert::AreEqual("[[1,2],[3,4]]"s, sum.as_string<char>());
	}

	TEST_METHOD(AdditionToDefaultConstructed)
	{
		auto val = Value{ 9,9 };
		Assert::AreEqual(val, val + Value{});
		Assert::AreEqual(val, Value{} + val);

		val += Value{};
		Assert::AreEqual(Value{9, 9}, val);
	}

	TEST_METHOD(AdditionComplex)
	{
		auto sum = Value::from_string("[[1,2],[[3,4],[5,6]]]"s) + Value::from_string("[1,[2,[[3,4],5]]]"s);
		Assert::AreEqual("[[[1,2],[[3,4],[5,6]]],[1,[5,[0,9]]]]"s, sum.as_string<char>());
	}

	TEST_METHOD(ExplodeSample1)
	{
		auto v = Value::from_string("[[[[[9,8],1],2],3],4]");

		Assert::IsTrue(detail::ValueExploder{ v }.explode());
		Assert::AreEqual("[[[[0,9],2],3],4]"s, v.as_string<char>());

		Assert::IsFalse(detail::ValueExploder{ v }.explode());
	}

	TEST_METHOD(ExplodeSample2)
	{
		auto v = Value::from_string("[7,[6,[5,[4,[3,2]]]]]");

		Assert::IsTrue(detail::ValueExploder{ v }.explode());
		Assert::AreEqual("[7,[6,[5,[7,0]]]]"s, v.as_string<char>());

		Assert::IsFalse(detail::ValueExploder{ v }.explode());
	}

	TEST_METHOD(ExplodeSample3)
	{
		auto v = Value::from_string("[[6,[5,[4,[3,2]]]],1]");

		Assert::IsTrue(detail::ValueExploder{ v }.explode());
		Assert::AreEqual("[[6,[5,[7,0]]],3]"s, v.as_string<char>());

		Assert::IsFalse(detail::ValueExploder{ v }.explode());
	}

	TEST_METHOD(ExplodeSample4)
	{
		auto v = Value::from_string("[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]");

		Assert::IsTrue(detail::ValueExploder{ v }.explode());
		Assert::AreEqual("[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]"s, v.as_string<char>());

		Assert::IsTrue(detail::ValueExploder{ v }.explode());
		Assert::AreEqual("[[3,[2,[8,0]]],[9,[5,[7,0]]]]"s, v.as_string<char>());

		Assert::IsFalse(detail::ValueExploder{ v }.explode());
	}

	TEST_METHOD(SplitSample1)
	{
		auto v = Value::from_string("[[[[0,7],4],[15,[0,13]]],[1,1]]");
		
		Assert::IsTrue(detail::ValueSplitter{ v }.split());
		Assert::AreEqual("[[[[0,7],4],[[7,8],[0,13]]],[1,1]]"s, v.as_string<char>());

		Assert::IsTrue(detail::ValueSplitter{ v }.split());
		Assert::AreEqual("[[[[0,7],4],[[7,8],[0,[6,7]]]],[1,1]]"s, v.as_string<char>());

		Assert::IsFalse(detail::ValueSplitter{ v }.split());
	}

	TEST_METHOD(SumSampleNumbers1)
	{
		const auto v1 = Value::from_string("[[[[4,3],4],4],[7,[[8,4],9]]]");
		const auto v2 = Value::from_string("[1,1]");

		const auto sum = v1 + v2;

		Assert::AreEqual("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]"s, sum.as_string<char>());
	}

	TEST_METHOD(SumSampleListOfNumbers1)
	{
		const auto numbers = {
			Value{1, 1},
			Value{2, 2},
			Value{3, 3},
			Value{4, 4}
		};

		const auto sum = std::accumulate(numbers.begin(), numbers.end(), Value{});

		Assert::AreEqual(Value::from_string("[[[[1,1],[2,2]],[3,3]],[4,4]]"), sum);
	}

	TEST_METHOD(SumSampleListOfNumbers2)
	{
		const auto numbers = {
			Value{1, 1},
			Value{2, 2},
			Value{3, 3},
			Value{4, 4},
			Value{5, 5}
		};

		const auto sum = std::accumulate(numbers.begin(), numbers.end(), Value{});

		Assert::AreEqual(Value::from_string("[[[[3,0],[5,3]],[4,4]],[5,5]]"), sum);
	}

	TEST_METHOD(SumSampleListOfNumbers3)
	{
		const auto numbers = {
			Value{1, 1},
			Value{2, 2},
			Value{3, 3},
			Value{4, 4},
			Value{5, 5},
			Value{6, 6}
		};

		const auto sum = std::accumulate(numbers.begin(), numbers.end(), Value{});

		Assert::AreEqual(Value::from_string("[[[[5,0],[7,4]],[5,5]],[6,6]]"), sum);
	}

	TEST_METHOD(SumSampleListOfNumbers4)
	{
		const auto numbers = {
			Value::from_string("[[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]]"),
			Value::from_string("[7,[[[3,7],[4,3]],[[6,3],[8,8]]]]"),
			Value::from_string("[[2,[[0,8],[3,4]]],[[[6,7],1],[7,[1,6]]]]"),
			Value::from_string("[[[[2,4],7],[6,[0,5]]],[[[6,8],[2,8]],[[2,1],[4,5]]]]"),
			Value::from_string("[7,[5,[[3,8],[1,4]]]]"),
			Value::from_string("[[2,[2,2]],[8,[8,1]]]"),
			Value::from_string("[2,9]"),
			Value::from_string("[1,[[[9,3],9],[[9,0],[0,7]]]]"),
			Value::from_string("[[[5,[7,4]],7],1]"),
			Value::from_string("[[[[4,2],2],6],[8,7]]")
		};

		const auto sum = std::accumulate(numbers.begin(), numbers.end(), Value{});

		Assert::AreEqual(Value::from_string("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]"), sum);
	}
};
}