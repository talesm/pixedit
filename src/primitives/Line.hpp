#ifndef PIXEDIT_SRC_PRIMITIVES_LINE_INCLUDED
#define PIXEDIT_SRC_PRIMITIVES_LINE_INCLUDED

#include <SDL.h>
#include "Canvas.hpp"

namespace pixedit {

struct HorizontalLine
{
  int x;
  int y;
  int length;
};

struct LineTo
{
  int x1, y1, x2, y2;
  LineTo(int x1, int y1, int x2, int y2)
    : x1(x1)
    , y1(y1)
    , x2(x2)
    , y2(y2)
  {
  }
  LineTo(const SDL_Point& p1, const SDL_Point& p2)
    : LineTo(p1.x, p1.y, p2.x, p2.y)
  {
  }
};

struct OpenLineTo : LineTo
{
  using LineTo::LineTo;
};

struct VerticalLine
{
  int x;
  int y;
  int length;
};

inline Canvas&
operator|(Canvas& c, VerticalLine l)
{
  return c | SDL_Rect(l.x, l.y, 1, l.length);
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_PRIMITIVES_LINE_INCLUDED */
