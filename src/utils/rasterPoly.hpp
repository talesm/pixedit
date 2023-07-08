#ifndef PIXEDIT_SRC_UTILS_RASTER_POLY_INCLUDED
#define PIXEDIT_SRC_UTILS_RASTER_POLY_INCLUDED

#include <algorithm>
#include <set>
#include <vector>
#include "rasterLine.hpp"

namespace pixedit {

template<class CALLBACK>
void
rasterPoly(const int points[], size_t count, CALLBACK callback)
{
  if (count < 3) return;
  int minY = points[1], maxY = points[1];
  for (size_t i = 1; i < count; ++i) {
    int curr = points[i * 2 + 1];
    if (curr < minY) minY = curr;
    if (curr > maxY) maxY = curr;
  }
  std::vector<int> ints;
  for (int y = minY; y <= maxY; ++y) {
    ints.clear();
    for (size_t i = 0; i < count; ++i) {
      int ind1 = i > 0 ? i - 1 : count - 1;
      int ind2 = i;
      int x1 = points[ind1 * 2];
      int y1 = points[ind1 * 2 + 1];
      int x2 = points[ind2 * 2];
      int y2 = points[ind2 * 2 + 1];
      if (y1 > y2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
      } else if (y1 == y2) {
        continue;
      }
      if (((y >= y1) && (y < y2)) || ((y == maxY) && (y > y1) && (y <= y2))) {
        ints.push_back(((65536 * (y - y1)) / (y2 - y1)) * (x2 - x1) +
                       (65536 * x1));
      }
    }
    std::sort(ints.begin(), ints.end());
    for (size_t i = 0; i < ints.size(); i += 2) {
      int xA = ints[i] + 1;
      xA = (xA >> 16) + ((xA & 32768) >> 15);
      int xB = ints[i + 1] - 1;
      xB = (xB >> 16) + ((xB & 32768) >> 15);
      callback(xA, y, xB - xA + 1);
    }
  }
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_RASTER_POLY_INCLUDED */
