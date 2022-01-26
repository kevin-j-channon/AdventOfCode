#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"

namespace test_common
{
TEST_CLASS(Point2D)
{
public:

	TEST_METHOD(CanBeAdded)
	{
		const auto v1 = aoc::Point2D<int>{ 1, 2 };
		const auto v2 = aoc::Point2D<int>{ 3, 4 };

		const auto v3 = v1 + v2;

		Assert::AreEqual(4, v3.x);
		Assert::AreEqual(6, v3.y);
	}

	TEST_METHOD(StreamExtractionWorksForValidStream)
	{
		std::stringstream ss("1,2");
		auto v = aoc::Point2D<uint32_t>{};

		ss >> v;

		Assert::AreEqual(uint32_t{ 1 }, v.x);
		Assert::AreEqual(uint32_t{ 2 }, v.y);
	}

	TEST_METHOD(StreamExtractionFailsIfMalformed)
	{
		std::stringstream ss("09");
		auto v = aoc::Point2D<uint32_t>{};

		Assert::ExpectException<aoc::Exception>([&]() { ss >> v; });

		Assert::IsTrue(ss.fail());
	}

	TEST_METHOD(StreamExtractionFailsIfThereAreNonnumericElements)
	{
		std::stringstream ss("0,X");
		auto v = aoc::Point2D<uint32_t>{};

		Assert::ExpectException<aoc::Exception>([&]() { ss >> v; });

		Assert::IsTrue(ss.fail());
	}
};

TEST_CLASS(Line2d)
{
public:
	TEST_METHOD(StreamExtractionWorksForValidStream)
	{
		std::stringstream ss("0,9 -> 5,7");
		auto line = aoc::Line2d<uint32_t>{};

		ss >> line;

		Assert::AreEqual(uint32_t{ 0 }, line.start.x);
		Assert::AreEqual(uint32_t{ 9 }, line.start.y);
		Assert::AreEqual(uint32_t{ 5 }, line.finish.x);
		Assert::AreEqual(uint32_t{ 7 }, line.finish.y);
	}

	TEST_METHOD(StreamExtractionFailsIfStartIsMalformed)
	{
		std::stringstream ss("09 -> 5,7");
		auto line = aoc::Line2d<uint32_t>{};

		Assert::ExpectException<aoc::Exception>([&]() { ss >> line; });

		Assert::IsTrue(ss.fail());
	}

	TEST_METHOD(StreamExtractionFailsIfFinishIsMalformed)
	{
		std::stringstream ss("0,9 -> 57");
		auto line = aoc::Line2d<uint32_t>{};

		Assert::ExpectException<aoc::Exception>([&]() { ss >> line; });

		Assert::IsTrue(ss.fail());
	}

	TEST_METHOD(StreamExtractionFailsIfArrowIsMalformed)
	{
		std::stringstream ss("0,9 , 5,7");
		auto line = aoc::Line2d<uint32_t>{};

		Assert::ExpectException<aoc::Exception>([&]() { ss >> line; });

		Assert::IsTrue(ss.fail());
	}

	TEST_METHOD(StreamExtractionFailsIfThereAreNonnumericElements)
	{
		std::stringstream ss("0,X , 5,7");
		auto line = aoc::Line2d<uint32_t>{};

		Assert::ExpectException<aoc::Exception>([&]() { ss >> line; });

		Assert::IsTrue(ss.fail());
	}

	TEST_METHOD(ExtractMoreThanOneLineFromStream)
	{
		std::stringstream ss("0,9 -> 5,7\n8,0 -> 0,8");
		auto line1 = aoc::Line2d<uint32_t>{};
		auto line2 = aoc::Line2d<uint32_t>{};

		ss >> line1 >> line2;

		Assert::AreEqual(uint32_t{ 0 }, line1.start.x);
		Assert::AreEqual(uint32_t{ 9 }, line1.start.y);
		Assert::AreEqual(uint32_t{ 5 }, line1.finish.x);
		Assert::AreEqual(uint32_t{ 7 }, line1.finish.y);

		Assert::AreEqual(uint32_t{ 8 }, line2.start.x);
		Assert::AreEqual(uint32_t{ 0 }, line2.start.y);
		Assert::AreEqual(uint32_t{ 0 }, line2.finish.x);
		Assert::AreEqual(uint32_t{ 8 }, line2.finish.y);
	}

	TEST_METHOD(FromMethodReadsFromStream)
	{
		std::stringstream ss("7,0 -> 7,4");

		const auto line = aoc::Line2d<uint32_t>{}.from(ss);

		Assert::AreEqual(uint32_t{ 7 }, line.start.x);
		Assert::AreEqual(uint32_t{ 0 }, line.start.y);
		Assert::AreEqual(uint32_t{ 7 }, line.finish.x);
		Assert::AreEqual(uint32_t{ 4 }, line.finish.y);
	}

	TEST_METHOD(IsVerticalIdentifiesAnUpwardVerticalLine)
	{
		Assert::IsTrue(aoc::is_vertical(aoc::Line2d<uint32_t>{ {7, 0}, { 7, 4 } }));
	}

	TEST_METHOD(IsVerticalIdentifiesADownwardVerticalLine)
	{
		Assert::IsTrue(aoc::is_vertical(aoc::Line2d<uint32_t>{ {10, 5}, { 10, 1 } }));
	}

	TEST_METHOD(IsVerticalIsFalseForNonVerticalLines)
	{
		Assert::IsFalse(aoc::is_vertical(aoc::Line2d<uint32_t>{ {10, 5}, { 9, 1 }}));
	}

	TEST_METHOD(IsHorizontalIdentifiesALeftHorizontalLine)
	{
		Assert::IsTrue(aoc::is_horizontal(aoc::Line2d<uint32_t>{ {7, 0}, { 10, 0 }}));
	}

	TEST_METHOD(IsHorizontalIdentifiesARightHorizontalLine)
	{
		Assert::IsTrue(aoc::is_horizontal(aoc::Line2d<uint32_t>{ {10, 5}, { 4, 5 }}));
	}

	TEST_METHOD(IsHorizontalIsFalseForNonVerticalLines)
	{
		Assert::IsFalse(aoc::is_horizontal(aoc::Line2d<uint32_t>{ {3, 5}, { 9, 1 }}));
	}

	TEST_METHOD(IsDiagonalIdentifiesUpRightDiagonalLine)
	{
		Assert::IsTrue(aoc::is_diagonal(aoc::Line2d<uint32_t>{ {1, 1}, { 5, 5 }}));
	}

	TEST_METHOD(IsDiagonalIdentifiesUpLeftDiagonalLine)
	{
		Assert::IsTrue(aoc::is_diagonal(aoc::Line2d<uint32_t>{ {9, 7}, { 5, 11 }}));
	}

	TEST_METHOD(IsDiagonalIdentifiesDownRightDiagonalLine)
	{
		Assert::IsTrue(aoc::is_diagonal(aoc::Line2d<uint32_t>{ {1, 6}, { 6, 1 }}));
	}

	TEST_METHOD(IsDiagonalIdentifiesDownLeftDiagonalLine)
	{
		Assert::IsTrue(aoc::is_diagonal(aoc::Line2d<uint32_t>{ {6, 6}, { 1, 1 }}));
	}

	TEST_METHOD(IsDiagonalIsFalseForNonDiagonalLine)
	{
		Assert::IsFalse(aoc::is_diagonal(aoc::Line2d<uint32_t>{ {1, 1}, { 3, 5 }}));
	}

	TEST_METHOD(RasterizeVerticalLinesWorks)
	{
		using Line_t = aoc::Line2d<uint32_t>;
		const auto points = aoc::rasterize<Line_t::horizontal | Line_t::vertical>(Line_t{ {1, 1}, { 1, 5 } });

		const auto expected_points = std::vector<aoc::Point2D<uint32_t>>{ {1,1}, {1,2}, {1,3}, {1,4}, {1,5} };

		Assert::IsTrue(std::equal(expected_points.begin(), expected_points.end(), points.begin()));
	}
	TEST_METHOD(RasterizeHorizontalLinesWorks)
	{
		using Line_t = aoc::Line2d<uint32_t>;
		const auto points = aoc::rasterize<Line_t::horizontal | Line_t::vertical>(Line_t{ {1, 1}, { 5, 1 } });

		const auto expected_points = std::vector<aoc::Point2D<uint32_t>>{ {1,1}, {2,1}, {3,1}, {4,1}, {5,1} };

		Assert::IsTrue(std::equal(expected_points.begin(), expected_points.end(), points.begin()));
	}

	TEST_METHOD(RasterizeThrowsForNonHorizontalOrVerticalLines)
	{
		using Line_t = aoc::Line2d<uint32_t>;
		Assert::ExpectException<aoc::Exception>([]() {
			aoc::rasterize<Line_t::horizontal | Line_t::vertical>(Line_t{ {1, 2}, { 5, 1 } });
			});
	}

	TEST_METHOD(RasterizeDownwardVerticalLinesWorks)
	{
		using Line_t = aoc::Line2d<uint32_t>;
		const auto points = aoc::rasterize<Line_t::horizontal | Line_t::vertical>(Line_t{ {1, 5}, { 1, 1 } });

		const auto expected_points = std::vector<aoc::Point2D<uint32_t>>{ {1,1}, {1,2}, {1,3}, {1,4}, {1,5} };

		Assert::IsTrue(std::equal(expected_points.begin(), expected_points.end(), points.begin()));
	}

	TEST_METHOD(RasterizeLeftHorizontalLinesWorks)
	{
		using Line_t = aoc::Line2d<uint32_t>;
		const auto points = aoc::rasterize<Line_t::horizontal | Line_t::vertical>(Line_t{ {5, 1}, { 1, 1 } });

		const auto expected_points = std::vector<aoc::Point2D<uint32_t>>{ {1,1}, {2,1}, {3,1}, {4,1}, {5,1} };

		Assert::IsTrue(std::equal(expected_points.begin(), expected_points.end(), points.begin()));
	}
};

TEST_CLASS(TestRectangle)
{
public:
	TEST_METHOD(DefaultConstructToZeroSize)
	{
		const auto rect = aoc::Rectangle<size_t>{};
		Assert::AreEqual(size_t{ 0 }, rect.top_left().x);
		Assert::AreEqual(size_t{ 0 }, rect.top_left().y);
		Assert::AreEqual(size_t{ 0 }, rect.bottom_right().x);
		Assert::AreEqual(size_t{ 0 }, rect.bottom_right().y);
	}
};
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

	TEST_METHOD(BeginIteratorIsMin)
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
}
