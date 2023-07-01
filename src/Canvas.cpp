#include "Canvas.hpp"
#include "algorithms/surface.hpp"

namespace pixedit {

void
Canvas::setSurface(SDL_Surface* value)
{
  surface = value;
}

Canvas&
operator|(Canvas& c, SDL_Point p)
{
  Uint32 color = c.colorA;
  if (c.pattern) {
    int xx = p.x % 8;
    int yy = p.y % 8;
    if ((c.pattern >> (yy * 8 + xx)) & 1) { color = c.colorB; }
  }
  setPixelAt(c.surface, p.x, p.y, color);
  return c;
}

Canvas&
operator|(Canvas& c, drawHorizontalLine l)
{
  if (!c.pattern) return c | SDL_Rect{l.x, l.y, l.length, 1};
  for (int i = 0; i < l.length; ++i, ++l.x) c | SDL_Point{l.x, l.y};
  return c;
}

Canvas&
operator|(Canvas& c, SDL_Rect rect)
{
  if (c.pattern) {
    for (int i = 0; i < rect.h; ++i, ++rect.y)
      c | drawHorizontalLine(rect.x, rect.y, rect.w);
  } else {
    SDL_FillRect(c.surface, &rect, c.colorA);
  }
  return c;
}

} // namespace pixedit
