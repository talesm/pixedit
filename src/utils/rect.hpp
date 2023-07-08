#ifndef PIXEDIT_SRC_UTILS_RECT_INCLUDED
#define PIXEDIT_SRC_UTILS_RECT_INCLUDED

#include <SDL.h>

namespace pixedit {
constexpr SDL_Rect
fromPoints(const SDL_Point& a, const SDL_Point& b)
{
  SDL_Rect rect;
  if (a.x < b.x) {
    rect.x = a.x;
    rect.w = b.x - a.x + 1;
  } else {
    rect.x = b.x;
    rect.w = a.x - b.x + 1;
  }
  if (a.y < b.y) {
    rect.y = a.y;
    rect.h = b.y - a.y + 1;
  } else {
    rect.y = b.y;
    rect.h = a.y - b.y + 1;
  }
  return rect;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_RECT_INCLUDED */