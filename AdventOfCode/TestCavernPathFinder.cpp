#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "CavernPathFinder.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace test_cavern_path_finder
{
TEST_CLASS(TestCavern)
{
public:
	TEST_METHOD(LoadFromStream)
	{
		std::stringstream data(
			"1163751742\n"
			"1381373672\n"
			"2136511328\n"
			"3694931569\n"
			"7463417111\n"
			"1319128137\n"
			"1359912421\n"
			"3125421639\n"
			"1293138521\n"
			"2311944581"
		);

		const auto cavern = aoc::navigation::Cavern{ data };

		const auto& risks = cavern.risk_grid();
		Assert::AreEqual(aoc::navigation::Cavern::Size_t{ 10 }, risks.n_rows);
		Assert::AreEqual(aoc::navigation::Cavern::Size_t{ 10 }, risks.n_cols);

		constexpr auto expected = std::array{
			std::array{1,1,6,3,7,5,1,7,4,2},
			std::array{1,3,8,1,3,7,3,6,7,2},
			std::array{2,1,3,6,5,1,1,3,2,8},
			std::array{3,6,9,4,9,3,1,5,6,9},
			std::array{7,4,6,3,4,1,7,1,1,1},
			std::array{1,3,1,9,1,2,8,1,3,7},
			std::array{1,3,5,9,9,1,2,4,2,1},
			std::array{3,1,2,5,4,2,1,6,3,9},
			std::array{1,2,9,3,1,3,8,5,2,1},
			std::array{2,3,1,1,9,4,4,5,8,1}
		};

		for (auto r = 0u; r < 10; ++r) {
			for (auto c = 0u; c < 10; ++c) {
				Assert::AreEqual(expected[r][c], risks.at(r, c), std::format(L"Mis-match at row {}, col {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(ExpandSmallCave)
	{
		std::stringstream data("56\n78");

		const auto risks = aoc::navigation::Cavern{ data }.expand(2).risk_grid();

		Assert::AreEqual(aoc::navigation::Cavern::Size_t{ 4 }, risks.n_rows);
		Assert::AreEqual(aoc::navigation::Cavern::Size_t{ 4 }, risks.n_cols);

		constexpr auto expected_risks = std::array{
			std::array{5,6,6,7},
			std::array{7,8,8,9},
			std::array{6,7,7,8},
			std::array{8,9,9,1}
		};

		for (auto r = 0u; r < 4; ++r) {
			for (auto c = 0u; c < 4; ++c) {
				Assert::AreEqual(expected_risks[r][c], risks.at(r, c), std::format(L"Mismatch at {}, {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(ExpandExampleCase)
	{
		std::stringstream data(
			"1163751742\n"
			"1381373672\n"
			"2136511328\n"
			"3694931569\n"
			"7463417111\n"
			"1319128137\n"
			"1359912421\n"
			"3125421639\n"
			"1293138521\n"
			"2311944581"
		);

		const auto expanded_risks = aoc::navigation::Cavern{ data }.expand(5).risk_grid();

		constexpr auto expected_data_str =
			"11637517422274862853338597396444961841755517295286\n"
			"13813736722492484783351359589446246169155735727126\n"
			"21365113283247622439435873354154698446526571955763\n"
			"36949315694715142671582625378269373648937148475914\n"
			"74634171118574528222968563933317967414442817852555\n"
			"13191281372421239248353234135946434524615754563572\n"
			"13599124212461123532357223464346833457545794456865\n"
			"31254216394236532741534764385264587549637569865174\n"
			"12931385212314249632342535174345364628545647573965\n"
			"23119445813422155692453326671356443778246755488935\n"
			"22748628533385973964449618417555172952866628316397\n"
			"24924847833513595894462461691557357271266846838237\n"
			"32476224394358733541546984465265719557637682166874\n"
			"47151426715826253782693736489371484759148259586125\n"
			"85745282229685639333179674144428178525553928963666\n"
			"24212392483532341359464345246157545635726865674683\n"
			"24611235323572234643468334575457944568656815567976\n"
			"42365327415347643852645875496375698651748671976285\n"
			"23142496323425351743453646285456475739656758684176\n"
			"34221556924533266713564437782467554889357866599146\n"
			"33859739644496184175551729528666283163977739427418\n"
			"35135958944624616915573572712668468382377957949348\n"
			"43587335415469844652657195576376821668748793277985\n"
			"58262537826937364893714847591482595861259361697236\n"
			"96856393331796741444281785255539289636664139174777\n"
			"35323413594643452461575456357268656746837976785794\n"
			"35722346434683345754579445686568155679767926678187\n"
			"53476438526458754963756986517486719762859782187396\n"
			"34253517434536462854564757396567586841767869795287\n"
			"45332667135644377824675548893578665991468977611257\n"
			"44961841755517295286662831639777394274188841538529\n"
			"46246169155735727126684683823779579493488168151459\n"
			"54698446526571955763768216687487932779859814388196\n"
			"69373648937148475914825958612593616972361472718347\n"
			"17967414442817852555392896366641391747775241285888\n"
			"46434524615754563572686567468379767857948187896815\n"
			"46833457545794456865681556797679266781878137789298\n"
			"64587549637569865174867197628597821873961893298417\n"
			"45364628545647573965675868417678697952878971816398\n"
			"56443778246755488935786659914689776112579188722368\n"
			"55172952866628316397773942741888415385299952649631\n"
			"57357271266846838237795794934881681514599279262561\n"
			"65719557637682166874879327798598143881961925499217\n"
			"71484759148259586125936169723614727183472583829458\n"
			"28178525553928963666413917477752412858886352396999\n"
			"57545635726865674683797678579481878968159298917926\n"
			"57944568656815567976792667818781377892989248891319\n"
			"75698651748671976285978218739618932984172914319528\n"
			"56475739656758684176786979528789718163989182927419\n"
			"67554889357866599146897761125791887223681299833479"
			;

		std::stringstream expected_data(expected_data_str);

		const auto expected_risks = aoc::navigation::Cavern{ expected_data }.risk_grid();

		Assert::AreEqual(expected_risks.n_rows, expanded_risks.n_rows);
		Assert::AreEqual(expected_risks.n_cols, expanded_risks.n_cols);

		for (auto r = 0u; r < expected_risks.n_rows; ++r) {
			for (auto c = 0u; c < expected_risks.n_cols; ++c) {
				Assert::AreEqual(expected_risks.at(r, c), expanded_risks.at(r, c), std::format(L"Mismatch at {}, {}", r, c).c_str());
			}
		}
	}
};

TEST_CLASS(TestCavernPathFinder)
{
public:

	TEST_METHOD(SimplestCase)
	{
		auto risks = aoc::navigation::Cavern::Grid_t{ {1, 3}, {2, 1} };

		const auto route = aoc::navigation::CavernPathFinder{}.plot_course(risks).route();
		Assert::AreEqual(size_t{ 3 }, route.size());

		using Point_t = aoc::Point2D<aoc::navigation::Cavern::Size_t>;
		constexpr auto expected_route = std::array{ Point_t{0, 0}, Point_t{0, 1}, Point_t{1, 1} };
		auto expected = expected_route.begin();
		for (auto location = route.begin(); location != route.end(); ++location, ++expected) {
			Assert::AreEqual(expected->x, location->x);
			Assert::AreEqual(expected->y, location->y);
		}
	}

	TEST_METHOD(SimpleExampleRouteIsFound)
	{
		std::stringstream data(
			"1163751742\n"
			"1381373672\n"
			"2136511328\n"
			"3694931569\n"
			"7463417111\n"
			"1319128137\n"
			"1359912421\n"
			"3125421639\n"
			"1293138521\n"
			"2311944581"
		);

		const auto cavern = aoc::navigation::Cavern{ data };
		const auto route = aoc::navigation::CavernPathFinder{}.plot_course(cavern.risk_grid()).route();

		using Point_t = aoc::Point2D<aoc::navigation::Cavern::Size_t>;
		constexpr auto expected_route = std::array{
			Point_t{ 0, 0 },
			Point_t{ 0, 1 },
			Point_t{ 0, 2 },
			Point_t{ 1, 2 },
			Point_t{ 2, 2 },
			Point_t{ 3, 2 },
			Point_t{ 4, 2 },
			Point_t{ 5, 2 },
			Point_t{ 6, 2 },
			Point_t{ 6, 3 },
			Point_t{ 7, 3 },
			Point_t{ 7, 4 },
			Point_t{ 8, 4 },
			Point_t{ 8, 5 },
			Point_t{ 8, 6 },
			Point_t{ 8, 7 },
			Point_t{ 8, 8 },
			Point_t{ 9, 8 },
			Point_t{ 9, 9 }
		};

		Assert::AreEqual(expected_route.size(), route.size());
		auto expected = expected_route.begin();
		for (auto location = route.begin(); location != route.end(); ++location, ++expected) {
			Assert::AreEqual(expected->x, location->x);
			Assert::AreEqual(expected->y, location->y);
		}
	}

	TEST_METHOD(SimpleExampleScoreIsCorrect)
	{
		std::stringstream data(
			"1163751742\n"
			"1381373672\n"
			"2136511328\n"
			"3694931569\n"
			"7463417111\n"
			"1319128137\n"
			"1359912421\n"
			"3125421639\n"
			"1293138521\n"
			"2311944581"
		);

		const auto cavern = aoc::navigation::Cavern{ data };
		const auto score = aoc::navigation::CavernPathFinder{}.plot_course(cavern.risk_grid()).score();

		Assert::AreEqual(size_t{ 40 }, score);
	}

	TEST_METHOD(SimpleExampleScoreIsCorrectWhenExpanded)
	{
		std::stringstream data(
			"1163751742\n"
			"1381373672\n"
			"2136511328\n"
			"3694931569\n"
			"7463417111\n"
			"1319128137\n"
			"1359912421\n"
			"3125421639\n"
			"1293138521\n"
			"2311944581"
		);

		const auto score = aoc::navigation::CavernPathFinder{}.plot_course(aoc::navigation::Cavern(data).expand(5).risk_grid()).score();

		Assert::AreEqual(size_t{ 315 }, score);
	}
};

TEST_CLASS(LongTestsOnFullData)
{
public:

	TEST_METHOD(Part2_mike)
	{
		std::ifstream data_file(DATA_DIR / "Day15_input_mike.txt");
		Assert::IsTrue(data_file.is_open());

		const auto score = aoc::navigation::CavernPathFinder{}.plot_course(aoc::navigation::Cavern{ data_file }.expand(5).risk_grid()).score();

		Assert::AreEqual(size_t{ 2835 }, score);
	}

	TEST_METHOD(Part2_compareExpanded)
	{
		std::ifstream data_file(DATA_DIR / "Day15_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto expanded_risks = aoc::navigation::Cavern{ data_file }.expand(5).risk_grid();

		std::ifstream ref_data_file(DATA_DIR / "Day15_expanded_cave_mike.txt");
		Assert::IsTrue(ref_data_file.is_open());

		const auto reference_risks = aoc::navigation::Cavern{ ref_data_file }.risk_grid();

		Assert::AreEqual(reference_risks.n_rows, expanded_risks.n_rows);
		Assert::AreEqual(reference_risks.n_cols, expanded_risks.n_cols);

		for (auto r = 0u; r < expanded_risks.n_rows; ++r) {
			for (auto c = 0u; c < expanded_risks.n_cols; ++c) {
				Assert::AreEqual(reference_risks.at(r, c), expanded_risks.at(r, c), std::format(L"Mismatch at ({}, {})", r, c).c_str());
			}
		}
	}

	TEST_METHOD(ExpandedGraphIsCorrect)
	{
		std::ifstream data_file(DATA_DIR / "Day15_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto risks = aoc::navigation::Cavern{ data_file }.expand(5).risk_grid();
		auto path_finder = aoc::navigation::CavernPathFinder{};
		const auto graph = path_finder.build_graph(risks);

		Assert::AreEqual(risks.n_elem, boost::num_vertices(graph));

		for (auto [vertex, end] = boost::vertices(graph); vertex != end; ++vertex) {
			const auto location = path_finder.cavern_location_from_vertex(*vertex);

			// Check location to the right, if this is not in the rightmost column
			if (location.x < risks.n_cols - 1) {
				const auto dest = path_finder.vertex_from_cavern_location({ location.x + 1, location.y });
				const auto edge_descriptor = boost::edge(*vertex, dest, graph);
				const auto weight = boost::get(boost::edge_weight, graph, edge_descriptor.first);

				Assert::AreEqual(risks.at(location.y, location.x + 1), weight,
					std::format(L"Weight mismatch on edge ({},{}) --{}-> ({}, {})",
						location.x, location.y, weight, location.x + 1, location.y).c_str());
			}

			// Check location underneath, if this is not in the bottom row
			if (location.y < risks.n_rows - 1) {
				const auto dest = path_finder.vertex_from_cavern_location({ location.x, location.y + 1 });
				const auto edge_descriptor = boost::edge(*vertex, dest, graph);
				const auto weight = boost::get(boost::edge_weight, graph, edge_descriptor.first);

				Assert::AreEqual(risks.at(location.y + 1, location.x), weight,
					std::format(L"Weight mismatch on edge ({},{}) --{}-> ({}, {})",
						location.x, location.y, weight, location.x, location.y + 1).c_str());
			}
		}
	}
};
}
