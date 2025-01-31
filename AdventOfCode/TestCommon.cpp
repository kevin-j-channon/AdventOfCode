#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"

template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<typename aoc::PairwiseCombinationIterator<class std::initializer_list<int>>>(const typename aoc::PairwiseCombinationIterator<class std::initializer_list<int>>& value)
{
	return L"";
}

template<>
inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<struct std::pair<int const*, int const*>>(const struct std::pair<int const*, int const*>& value)
{
	return std::format(L"({}, {})", *value.first, *value.second);
}

namespace test_common
{

TEST_CLASS(TestValueRange)
{
public:

	TEST_METHOD(StreamExtractionWorksForValidStream)
	{
		std::stringstream ss("1..2");
		auto v = aoc::ValueRange<uint32_t>{};

		ss >> v;

		Assert::AreEqual(uint32_t{ 1 }, v.min());
		Assert::AreEqual(uint32_t{ 2 }, v.max());
	}

	TEST_METHOD(ConstructRangeWithOutOfOrderMinMaxFails)
	{
		std::stringstream values("10 9");
		auto min = uint32_t{ 0 };
		auto max = uint32_t{ 0 };

		values >> min >> max;

		Assert::ExpectException<aoc::InvalidArgException>([&]() { aoc::ValueRange<uint32_t>{min, max}; });
	}

	TEST_METHOD(ReadOutOfOrderMinMaxFromStreamFails)
	{
		std::stringstream data("10..9");
		auto range = aoc::ValueRange<uint32_t>{};
		Assert::ExpectException<aoc::IOException>([&]() { data >> range; });
	}

	TEST_METHOD(StreamExtractionFailsIfMalformed)
	{
		std::stringstream ss("09");
		auto v = aoc::ValueRange<uint32_t>{};

		Assert::ExpectException<aoc::IOException>([&]() { ss >> v; });

		Assert::IsTrue(ss.fail());
	}

	TEST_METHOD(StreamExtractionFailsIfThereAreNonnumericElements)
	{
		std::stringstream ss("0..X");
		auto v = aoc::ValueRange<uint32_t>{};

		Assert::ExpectException<aoc::IOException>([&]() { ss >> v; });

		Assert::IsTrue(ss.fail());
	}

	TEST_METHOD(StreamExtractionFailsIfMaxIsMissing)
	{
		std::stringstream ss("0..");
		auto v = aoc::ValueRange<uint32_t>{};

		Assert::ExpectException<aoc::IOException>([&]() { ss >> v; });

		Assert::IsTrue(ss.fail());
	}
};

TEST_CLASS(TestValueRangeIterator)
{
public:
	TEST_METHOD(BeginIsMin)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		Assert::AreEqual(uint32_t{ 123 }, *range.begin());
	}

	TEST_METHOD(PrefixIncrementOperatorWorksOnIterator)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		auto it = range.begin();

		for (auto step = 0; step < 10; ++step, ++it) {
			Assert::AreEqual(range.min() + step, *it);
		}
	}

	TEST_METHOD(PostfixIncrementOperatorWorksOnIterator)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		auto it = range.begin();

		for (auto step = 0; step < 10; ++step) {
			Assert::AreEqual(range.min() + step, *it++);
		}
	}

	TEST_METHOD(PrefixDecrementOperatorWorksOnIterator)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		auto it = range.begin();

		for (auto step = 0; step < 10; ++step, ++it);

		for (auto step = 10; step != 0; --step, --it) {
			Assert::AreEqual(range.min() + step, *it);
		}
	}

	TEST_METHOD(PostfixDecrementOperatorWorksOnIterator)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		auto it = range.begin();

		for (auto step = 0; step < 10; ++step, ++it);

		for (auto step = 10; step != 0; --step) {
			Assert::AreEqual(range.min() + step, *it--);
		}
	}

	TEST_METHOD(AdditionOfRandomAccessWorks)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		auto it = range.begin();

		Assert::AreEqual(range.min() + 5, *(it + 5));
	}

	TEST_METHOD(SubtractFromRandomAccessWorks)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		auto it = range.begin();

		for (auto step = 0; step < 10; ++step, ++it);

		Assert::AreEqual(range.min() + 7, *(it - 3));
	}

	TEST_METHOD(CompoundAssignmentIncrementOperatorWorks)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		auto it = range.begin();

		it += 8;
		Assert::AreEqual(range.min() + 8, *it);
	}

	TEST_METHOD(CompoundAssignmentDecrementOperatorWorks)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		auto it = range.begin();

		it += 8;
		Assert::AreEqual(range.min() + 8, *it);
	}

	TEST_METHOD(EqualityComparisonWorks)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		const auto it_1 = range.begin();
		const auto it_2 = range.begin();
		Assert::IsTrue(it_1 == it_2);

		const auto it_3 = range.begin() + 2;
		Assert::IsTrue(it_1 != it_3);
	}

	TEST_METHOD(LessThanComparisonWorks)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		const auto it_1 = range.begin();
		const auto it_2 = range.begin();
		Assert::IsFalse(it_1 < it_2);
		Assert::IsTrue(it_1 <= it_2);

		const auto it_3 = range.begin() + 2;
		Assert::IsTrue(it_1 < it_3);
		Assert::IsFalse(it_3 < it_1);
		Assert::IsFalse(it_3 <= it_1);
	}

	TEST_METHOD(GreaterThanComparisonWorks)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		const auto it_1 = range.begin();
		const auto it_2 = range.begin();
		Assert::IsFalse(it_1 > it_2);
		Assert::IsTrue(it_1 >= it_2);

		const auto it_3 = range.begin() + 2;
		Assert::IsFalse(it_1 > it_3);
		Assert::IsTrue(it_3 > it_1);
		Assert::IsTrue(it_3 >= it_1);
	}

	TEST_METHOD(StlIncrementAndComparisonFunctionsWork)
	{
		const auto range = aoc::ValueRange<uint32_t>{ 123, 456 };
		const auto it_1 = range.begin();

		Assert::AreEqual(range.min() + 4, *std::next(it_1, 4));
		
		auto it_2 = it_1;
		std::advance(it_2, 3);
		Assert::AreEqual(range.min() + 3, *it_2);
		Assert::AreEqual(0, std::distance(range.begin(), it_1));
		Assert::AreEqual(3, std::distance(it_1, it_2));
	}
};

TEST_CLASS(TestExp)
{
public:
	TEST_METHOD(PowerOf2)
	{
		Assert::AreEqual(size_t{ 1 }, aoc::Exp<2, 0>::value);
		Assert::AreEqual(size_t{ 2 }, aoc::Exp<2, 1>::value);
		Assert::AreEqual(size_t{ 4 }, aoc::Exp<2, 2>::value);
		Assert::AreEqual(size_t{ 8 }, aoc::Exp<2, 3>::value);
		Assert::AreEqual(size_t{ 16 }, aoc::Exp<2, 4>::value);
		Assert::AreEqual(size_t{ 32 }, aoc::Exp<2, 5>::value);
		Assert::AreEqual(size_t{ 64 }, aoc::Exp<2, 6>::value);
		Assert::AreEqual(size_t{ 128 }, aoc::Exp<2, 7>::value);
	}
};

TEST_CLASS(TestPairwiseCombinationIterator)
{
public:
	TEST_METHOD(ComparisonOperators)
	{
		const auto v = { 1, 2, 3, 4 };
		const auto it_0 = aoc::PairwiseCombinationIterator{ v };
		const auto it_1 = aoc::PairwiseCombinationIterator{ v };

		Assert::AreEqual(it_0, it_1);

		const auto v2 = { 1, 2, 3, 4 };
		const auto it_2 = aoc::PairwiseCombinationIterator{ v2 };
		Assert::AreNotEqual(it_0, it_2);
	}

	TEST_METHOD(Dereference)
	{
		const auto v = { 1, 2, 3, 4 };
		const auto it = aoc::PairwiseCombinationIterator{ v };

		Assert::AreEqual({ v.begin(), v.begin() }, *it);
	}

	TEST_METHOD(PreIncrement)
	{
		const auto v = { 1, 2, 3, 4 };
		auto it = aoc::PairwiseCombinationIterator{ v };

		Assert::IsTrue(std::pair{ 1, 1 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 1, 2 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 1, 3 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 1, 4 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 2, 1 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 2, 2 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 2, 3 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 2, 4 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 3, 1 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 3, 2 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 3, 3 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 3, 4 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 4, 1 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 4, 2 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 4, 3 } == std::pair{ *(*it).first, *(*it).second }); ++it;
		Assert::IsTrue(std::pair{ 4, 4 } == std::pair{ *(*it).first, *(*it).second }); ++it;

		Assert::AreEqual(aoc::PairwiseCombinationIterator<std::initializer_list<int>>{}, it);
	}

	TEST_METHOD(PostIncrement)
	{
		const auto v = { 1, 2, 3, 4 };
		auto it = aoc::PairwiseCombinationIterator{ v };

		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 1, 1 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 1, 2 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 1, 3 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 1, 4 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 2, 1 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 2, 2 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 2, 3 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 2, 4 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 3, 1 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 3, 2 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 3, 3 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 3, 4 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 4, 1 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 4, 2 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 4, 3 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }
		{ auto temp_it = it++; Assert::IsTrue(std::pair{ 4, 4 } == std::pair{ *(*temp_it).first, *(*temp_it).second }); }

		Assert::AreEqual(aoc::PairwiseCombinationIterator<std::initializer_list<int>>{}, it);
	}
};
}
