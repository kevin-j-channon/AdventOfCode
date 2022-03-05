#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <Maths/Geometry.hpp>
#include "Common.hpp"

#include <numbers>

#define REPEAT_TEST(n) for (auto _ = 0; _ < 10; ++_)

namespace test_geometry
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

	TEST_METHOD(ContainsReturnsTrueForPointInsideIt)
	{
		const auto rect = aoc::Rectangle<int>{ {-10, 10}, {10, -10} };

		for (auto x : aoc::ValueRange<int>(-10, 10)) {
			for (auto y : aoc::ValueRange<int>(-10, 10)) {
				Assert::IsTrue(rect.contains({ x, y }));
			}
		}
	}
};

TEST_CLASS(TestRotations)
{
public:

	TEST_METHOD(DoublePointIn3Dimensions)
	{
		using Point_t = aoc::Point3D<double>;
		using Direction_t = aoc::Direction_t<double>;

		const auto points = std::array{
			Point_t{0.0, 0.0, 1.0},
			Point_t{0.0, 1.0, 0.0},
			Point_t{0.0, 1.0, 1.0},
			Point_t{1.0, 0.0, 0.0},
			Point_t{1.0, 0.0, 1.0},
			Point_t{1.0, 1.0, 0.0},
			Point_t{1.0, 1.0, 1.0},
		};

		const auto rotation_axes = std::array{
			Direction_t{0.0, 0.0, 1.0},
			Direction_t{0.0, 1.0, 0.0},
			Direction_t{1.0, 0.0, 0.0},
			// aoc::Direction_t<double>{0.0, 1.0, 1.0},
			// aoc::Direction_t<double>{1.0, 0.0, 1.0},
			// aoc::Direction_t<double>{1.0, 1.0, 0.0},
			// aoc::Direction_t<double>{1.0, 1.0, 1.0},
		};

		const auto expected = std::array{
			std::array{
				Point_t{0.,0.,1.},
				Point_t{-1.,0.,0.},
				Point_t{-1.,0.,1.},
				Point_t{0.,1.,0.},
				Point_t{0.,1.,1.},
				Point_t{-1.,1.,0.},
				Point_t{-1.,1.,1.}
			},
			std::array{
				Point_t{1.,0.,0.},
				Point_t{0.,1.,0.},
				Point_t{1.,1.,0.},
				Point_t{0.,0.,-1.},
				Point_t{1.,0.,-1.},
				Point_t{0.,1.,-1.},
				Point_t{1.,1.,-1.}
			},
			std::array{
				Point_t{0.,-1.,0.},
				Point_t{0.,0.,1.},
				Point_t{0.,-1.,1.},
				Point_t{1.,0.,0.},
				Point_t{1.,-1.,0.},
				Point_t{1.,0.,1.},
				Point_t{1.,-1.,1.}
			}
		};

		auto expected_points = expected.begin();
		for (const auto& axis : rotation_axes) {
			auto expected_point = expected_points->begin();

			for (const auto& p : points) {
				const auto rotated = aoc::rotate(p, axis, std::numbers::pi / 2);

				Assert::AreEqual(expected_point->x, rotated.x, 0.001);
				Assert::AreEqual(expected_point->y, rotated.y, 0.001);
				Assert::AreEqual(expected_point->z, rotated.z, 0.001);

				++expected_point;
			}

			++expected_points;
		}
	}

	TEST_METHOD(IntegerPointIn3Dimensions)
	{
		using Point_t = aoc::Point3D<int>;
		using Direction_t = aoc::Direction_t<int>;

		const auto points = std::array{
			Point_t{0, 0, 1},
			Point_t{0, 1, 0},
			Point_t{0, 1, 1},
			Point_t{1, 0, 0},
			Point_t{1, 0, 1},
			Point_t{1, 1, 0},
			Point_t{1, 1, 1},
		};

		const auto rotation_axes = std::array{
			Direction_t{0, 0, 1},
			Direction_t{0, 1, 0},
			Direction_t{1, 0, 0},
			// aoc::Direction_t<double>{0, 1, 1},
			// aoc::Direction_t<double>{1, 0, 1},
			// aoc::Direction_t<double>{1, 1, 0},
			// aoc::Direction_t<double>{1, 1, 1},
		};

		const auto expected = std::array{
			std::array{
				Point_t{0,0,1},
				Point_t{-1,0,0},
				Point_t{-1,0,1},
				Point_t{0,1,0},
				Point_t{0,1,1},
				Point_t{-1,1,0},
				Point_t{-1,1,1}
			},
			std::array{
				Point_t{1,0,0},
				Point_t{0,1,0},
				Point_t{1,1,0},
				Point_t{0,0,-1},
				Point_t{1,0,-1},
				Point_t{0,1,-1},
				Point_t{1,1,-1}
			},
			std::array{
				Point_t{0,-1,0},
				Point_t{0,0,1},
				Point_t{0,-1,1},
				Point_t{1,0,0},
				Point_t{1,-1,0},
				Point_t{1,0,1},
				Point_t{1,-1,1}
			}
		};

		auto expected_points = expected.begin();
		for (const auto& axis : rotation_axes) {
			auto expected_point = expected_points->begin();

			for (const auto& p : points) {
				const auto rotated = aoc::rotate(p, aoc::quaternion::from_axis_and_angle(axis, std::numbers::pi / 2));

				Assert::AreEqual(expected_point->x, rotated.x);
				Assert::AreEqual(expected_point->y, rotated.y);
				Assert::AreEqual(expected_point->z, rotated.z);

				++expected_point;
			}

			++expected_points;
		}
	}

	TEST_METHOD(ConvertRotationToAxisAndAngle)
	{
		using std::numbers::pi;
		using Direction_t = aoc::Direction_t<double>;

		auto rng = std::mt19937_64{ 324235 };
		auto angle_gen = std::uniform_real_distribution<>{ 0.0, pi };
		auto axis_gen = std::uniform_real_distribution<>{ 0.0, 1.0 };

		REPEAT_TEST(100) {
			const auto angle = angle_gen(rng);

			const auto unnormed_x = axis_gen(rng);
			const auto unnormed_y = axis_gen(rng);
			const auto unnormed_z = axis_gen(rng);

			const auto norm = std::sqrt(unnormed_x * unnormed_x + unnormed_y * unnormed_y + unnormed_z * unnormed_z);

			const auto axis = boost::qvm::vec<double, 3>{{ unnormed_x / norm, unnormed_y / norm, unnormed_z / norm }};

			const auto [recovered_axis, recovered_angle] = aoc::quaternion::to_axis_and_angle(boost::qvm::rot_quat(axis, angle));

			Assert::AreEqual(angle, recovered_angle, 1e-10);
			Assert::AreEqual(axis.a[0], recovered_axis.x, 1e-10);
			Assert::AreEqual(axis.a[1], recovered_axis.y, 1e-10);
			Assert::AreEqual(axis.a[2], recovered_axis.z, 1e-10);
		}
	}
};

}
