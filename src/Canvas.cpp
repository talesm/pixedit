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
} // namespace pixedit
