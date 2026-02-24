#include "rasterPoly.hpp"
#include <doctest/doctest.h>
#include <tuple>
#include <vector>

using namespace pixedit;

TEST_CASE("Raster Poly")
{
  using HLine = std::tuple<int, int, int>;
  std::vector<HLine> hLines;
  SUBCASE("Triangle (1,0)(0,1)(1,2)")
  {
    int points[] = {1, 0, 0, 1, 2, 1};
    rasterPoly(points,
               [&](int x, int y, int len) { hLines.emplace_back(x, y, len); });

    REQUIRE(hLines.at(0) == std::tuple{1, 0, 1});
    REQUIRE(hLines.at(1) == std::tuple{0, 1, 3});
  }
}
