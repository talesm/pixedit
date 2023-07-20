#include "catch.hpp"
#include "utils/rasterLine.hpp"
#include <tuple>
#include <vector>

using namespace pixedit;

using Point = std::tuple<int, int>;
using PointVector = std::vector<Point>;

TEST_CASE("Test rasterLine", "[raster][line]")
{
  PointVector points;
  auto callback = [&points](int x, int y) { points.emplace_back(x, y); };
  SECTION("Begin == End")
  {
    rasterLineOpen(10, 10, 10, 10, callback);
    REQUIRE(points.empty());
  }
  SECTION("End is 3px to the right")
  {
    rasterLineOpen(10, 10, 13, 10, callback);
    REQUIRE(points.at(0) == Point{10, 10});
    REQUIRE(points.at(1) == Point{11, 10});
    REQUIRE(points.at(2) == Point{12, 10});
  }
  SECTION("End is 3px to the left")
  {
    rasterLineOpen(10, 10, 7, 10, callback);
    REQUIRE(points.at(0) == Point{10, 10});
    REQUIRE(points.at(1) == Point{9, 10});
    REQUIRE(points.at(2) == Point{8, 10});
  }
  SECTION("End is 3px down")
  {
    rasterLineOpen(10, 10, 10, 13, callback);
    REQUIRE(points.at(0) == Point{10, 10});
    REQUIRE(points.at(1) == Point{10, 11});
    REQUIRE(points.at(2) == Point{10, 12});
  }
  SECTION("End is 3px up")
  {
    rasterLineOpen(10, 10, 10, 7, callback);
    REQUIRE(points.at(0) == Point{10, 10});
    REQUIRE(points.at(1) == Point{10, 9});
    REQUIRE(points.at(2) == Point{10, 8});
  }
  SECTION("End is 3px down right")
  {
    rasterLineOpen(10, 10, 13, 13, callback);
    REQUIRE(points.at(0) == Point{10, 10});
    REQUIRE(points.at(1) == Point{11, 11});
    REQUIRE(points.at(2) == Point{12, 12});
  }
  SECTION("End is 3px down left")
  {
    rasterLineOpen(10, 10, 7, 13, callback);
    REQUIRE(points.at(0) == Point{10, 10});
    REQUIRE(points.at(1) == Point{9, 11});
    REQUIRE(points.at(2) == Point{8, 12});
  }
  SECTION("End is 3px up right")
  {
    rasterLineOpen(10, 10, 13, 7, callback);
    REQUIRE(points.at(0) == Point{10, 10});
    REQUIRE(points.at(1) == Point{11, 9});
    REQUIRE(points.at(2) == Point{12, 8});
  }
  SECTION("End is 3px up left")
  {
    rasterLineOpen(10, 10, 7, 7, callback);
    REQUIRE(points.at(0) == Point{10, 10});
    REQUIRE(points.at(1) == Point{9, 9});
    REQUIRE(points.at(2) == Point{8, 8});
  }
}
