#ifndef PIXEDIT_SRC_UTILS_RASTER_LINE_INCLUDED
#define PIXEDIT_SRC_UTILS_RASTER_LINE_INCLUDED

#include <concepts>
#include <cstdlib>

namespace pixedit {

/// @brief Implement half open line alorithm
/// @tparam CALLBACK any callable that accepts two coordinates x, y;
/// @param xBeg initial x
/// @param yBeg initial y
/// @param xEnd final x
/// @param yEnd final
/// @param callback the callback
///
/// Line algorithm: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
template<std::invocable<int, int> CALLBACK>
void
rasterLineOpen(int xBeg, int yBeg, int xEnd, int yEnd, CALLBACK callback)
{
  auto rasterOctant = [](int deltaX, int deltaY, auto callback) {
    if (deltaY == 0) {
      for (int x = 0; x < deltaX; ++x) callback(x, 0);
      return;
    }
    int error = 0;
    int y = 0;
    for (int x = 0; x < deltaX; ++x) {
      callback(x, y);
      error += deltaY;
      if (error > deltaX / 2) {
        y++;
        error -= deltaX;
      }
    }
  };
  auto rasterQuadrant = [&](int deltaX, int deltaY, auto callback) {
    if (deltaX >= deltaY) {
      rasterOctant(deltaX, deltaY, callback);
    } else {
      rasterOctant(deltaY, deltaX, [&](int y, int x) { callback(x, y); });
    }
  };

  int deltaX = xEnd - xBeg, deltaY = yEnd - yBeg;
  if (deltaY >= 0) {
    if (deltaX >= 0)
      rasterQuadrant(
        deltaX, deltaY, [&](int x, int y) { callback(xBeg + x, yBeg + y); });
    else
      rasterQuadrant(
        -deltaX, deltaY, [&](int x, int y) { callback(xBeg - x, yBeg + y); });
  } else {
    if (deltaX >= 0)
      rasterQuadrant(
        deltaX, -deltaY, [&](int x, int y) { callback(xBeg + x, yBeg - y); });
    else
      rasterQuadrant(
        -deltaX, -deltaY, [&](int x, int y) { callback(xBeg - x, yBeg - y); });
  }
}

template<std::invocable<int, int> CALLBACK>
void
rasterLine(int xBeg, int yBeg, int xEnd, int yEnd, CALLBACK callback)
{
  rasterLineOpen(xBeg, yBeg, xEnd, yEnd, callback);
  callback(xEnd, yEnd);
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_RASTER_LINE_INCLUDED */
