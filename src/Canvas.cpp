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
  setPixelAt(c.surface, p.x, p.y, c.pColor);
  return c;
}

Canvas&
operator|(Canvas& c, HorizontalLine l)
{
  return c | SDL_Rect{l.x, l.y, l.length, 1};
}

Canvas&
operator|(Canvas& c, SDL_Rect rect)
{
  SDL_FillRect(c.surface, &rect, c.pColor);
  return c;
}

} // namespace pixedit
