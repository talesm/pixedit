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
  return c | SDL_Rect{l.x, l.y, l.length, 1};
}

Canvas&
operator|(Canvas& c, SDL_Rect rect)
{
  SDL_FillRect(c.surface, &rect, c.colorA);
  return c;
}

} // namespace pixedit
