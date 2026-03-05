#ifndef PIXEDIT_SRC_UTILS_RECT_INCLUDED
#define PIXEDIT_SRC_UTILS_RECT_INCLUDED

#include <SDL3/SDL.h>
#include <SDL3pp/SDL3pp.h>

namespace pixedit {

using SDL::FPoint;
using SDL::FRect;
using SDL::Point;
using SDL::Rect;

constexpr Rect
makeRect(const Point& p1, const Point& p2)
{
  if (p2.x < p1.x) {
    int xx = p2.x;
    int ww = p1.x - p2.x + 1;
    if (p2.y < p1.y) return {xx, p2.y, ww, p1.y - p2.y + 1};
    return {xx, p1.y, ww, p2.y - p1.y + 1};
  }
  int xx = p1.x;
  int ww = p2.x - p1.x + 1;
  if (p2.y < p1.y) return {xx, p2.y, ww, p1.y - p2.y + 1};
  return {xx, p1.y, ww, p2.y - p1.y + 1};
}

constexpr Rect
intersectFromOrigin(Rect rect, const Point& ceil)
{
  if (rect.x < 0) {
    rect.w += rect.x;
    rect.x = 0;
  }
  if (rect.y < 0) {
    rect.h += rect.y;
    rect.y = 0;
  }
  if (rect.x + rect.w > ceil.x) { rect.w = ceil.x - rect.x; }
  if (rect.y + rect.h > ceil.y) { rect.h = ceil.y - rect.y; }
  return rect;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_RECT_INCLUDED */
