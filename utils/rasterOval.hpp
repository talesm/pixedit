#ifndef PIXEDIT_SRC_UTILS_RASTER_OVAL_INCLUDED
#define PIXEDIT_SRC_UTILS_RASTER_OVAL_INCLUDED

#include <algorithm>
#include <span>
#include <vector>
#include "rasterLine.hpp"

namespace pixedit {

template<std::invocable<int, int, int> CALLBACK>
void
drawQuadrants(int x,
              int y,
              int dx,
              int dy,
              bool oddX,
              bool oddY,
              CALLBACK callback)
{
  // Based on SDL_gfx implementation
  if (dy == 0) {
    if (dx == 0) {
      callback(x, y, 0);
    } else if (oddY) {
      int xpdx = x + dx + oddX - 1;
      int xmdx = x - dx;
      callback(xmdx, y, xpdx - xmdx);
    }
  } else {
    int xpdx = x + dx + oddX - 1;
    int xmdx = x - dx;
    int ypdy = y + dy + oddY - 1;
    int ymdy = y - dy;
    callback(xmdx, ypdy, xpdx - xmdx);
    callback(xmdx, ymdy, xpdx - xmdx);
  }
}

template<bool fill = true, std::invocable<int, int, int> CALLBACK>
void
rasterOval(int x, int y, int w, int h, CALLBACK callback)
{
  int rx = w / 2;
  bool oddX = w % 2;
  int ry = h / 2;
  bool oddY = h % 2;
  x += rx;
  y += ry;
  int OVERSCAN = 4;

  /*
   * Sanity check radii
   */
  if ((rx < 0) || (ry < 0)) { return; }

  /*
   * Special cases for rx=0 and/or ry=0: draw a hline/vline/pixel
   */
  if (rx == 0) {
    if (ry == 0) {
      callback(x, y, 0);
    } else {
      for (int i = y - ry; i <= y + ry; ++i) { callback(x, i, 0); }
    }
  } else if (ry == 0) {
    callback(x - rx, y, rx * 2 + oddX - 1);
  }

  /*
   * Adjust overscan
   */
  int rxi = rx;
  int ryi = ry;

  /*
   * Top/bottom center points.
   */
  int oldY = 0, scrY = 0;
  int oldX = rxi, scrX = rxi;
  drawQuadrants(x, y, rx, 0, oddX, oddY, callback);

  /* Midpoint ellipse algorithm with overdraw */
  rxi *= OVERSCAN;
  ryi *= OVERSCAN;
  int ry2 = ryi * ryi;
  int ry22 = ry2 + ry2;
  int rx2 = rxi * rxi;
  int rx22 = rx2 + rx2;
  int curY = 0;
  int curX = rxi;
  int deltaY = 0;
  int deltaX = ry22 * curX;

  /* Points in segment 1 */
  int error = rx2 - ry2 * rxi + ry2 / 4;
  while (deltaY <= deltaX) {
    curY++;
    deltaY += rx22;

    error += deltaY + rx2;
    if (error >= 0) {
      curX--;
      deltaX -= ry22;
      error -= deltaX;
    }

    scrY = curY / OVERSCAN;
    scrX = curX / OVERSCAN;
    if ((scrY != oldY && scrX == oldX) || (scrY != oldY && scrX != oldX)) {
      drawQuadrants(x, y, scrX, scrY, oddX, oddY, callback);
      oldY = scrY;
      oldX = scrX;
    }
  }

  /* Points in segment 2 */
  if (curX > 0) {
    int curYp1 = curY + 1;
    int curXm1 = curX - 1;
    error =
      rx2 * curY * curYp1 + ((rx2 + 3) / 4) + ry2 * curXm1 * curXm1 - ry2 * rx2;
    while (curX > 0) {
      curX--;
      deltaX -= ry22;

      error += ry2;
      error -= deltaX;

      if (error <= 0) {
        curY++;
        deltaY += rx22;
        error += deltaY;
      }

      scrY = curY / OVERSCAN;
      scrX = curX / OVERSCAN;
      if ((scrY != oldY && scrX == oldX) || (scrY != oldY && scrX != oldX)) {
        oldX--;
        for (; oldX >= scrX; oldX--) {
          drawQuadrants(x, y, oldX, scrY, oddX, oddY, callback);
          /* prevent overdraw */
          if (fill) { oldX = scrX - 1; }
        }
        oldY = scrY;
        oldX = scrX;
      }
    }

    /* Remaining points in vertical */
    if (!fill) {
      oldX--;
      for (; oldX >= 0; oldX--) {
        drawQuadrants(x, y, oldX, scrY, oddX, oddY, callback);
      }
    }
  }
}

template<std::invocable<int, int> CALLBACK>
void
rasterOvalOutline(int x, int y, int w, int h, CALLBACK callback)
{
  rasterOval<false>(x, y, w, h, [&](int x, int y, int d) {
    callback(x, y);
    if (d) callback(x + d, y);
  });
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_RASTER_OVAL_INCLUDED */
