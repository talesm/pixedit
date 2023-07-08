#include "rasterPoly.hpp"
#include <tuple>
#include <vector>
#include "../catch.hpp"

using namespace pixedit;

TEST_CASE("Raster Poly", "[utils]")
{
  using HLine = std::tuple<int, int, int>;
  std::vector<HLine> hLines;
  SECTION("Triangle (1,0)(0,1)(1,2)")
  {
    int points[] = {1, 0, 0, 1, 2, 1};
    rasterPoly(points, 3, [&](int x, int y, int len) {
      hLines.emplace_back(x, y, len);
    });

    REQUIRE(hLines.at(0) == std::tuple{1, 0, 1});
    REQUIRE(hLines.at(1) == std::tuple{0, 1, 3});
  }
}
