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
