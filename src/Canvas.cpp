#include "Canvas.hpp"
#include "algorithms/surface.hpp"

namespace pixedit {

void
Canvas::setSurface(SDL_Surface* value)
{
  surface = value;
}

void
drawPixel(SDL_Surface* surface,
          Uint64 pattern,
          SDL_Point p,
          Uint32 color,
          Uint32 colorB)
{
  if (pattern) {
    int xx = p.x % 8;
    int yy = p.y % 8;
    if ((pattern >> (yy * 8 + xx)) & 1) { color = colorB; }
  }
  setPixelAt(surface, p.x, p.y, color);
}

Canvas&
operator|(Canvas& c, SDL_Point p)
{
  if (c.pen.type == Pen::BOX) {
    int xBeg = p.x - (c.pen.w / 2 + c.pen.w % 2 - 1);
    int xEnd = xBeg + c.pen.w;
    int yBeg = p.y - (c.pen.h / 2 + c.pen.h % 2 - 1);
    int yEnd = yBeg + c.pen.h;
    for (int i = yBeg; i < yEnd; ++i) {
      for (int j = xBeg; j < xEnd; ++j) {
        drawPixel(c.surface, c.pattern, {j, i}, c.colorA, c.colorB);
      }
    }
  } else {
    drawPixel(c.surface, c.pattern, p, c.colorA, c.colorB);
  }
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
