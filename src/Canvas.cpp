#include "Canvas.hpp"
#include "algorithms/surface.hpp"

namespace pixedit {

void
Canvas::setSurface(SDL_Surface* value)
{
  surface = value;
}

void
doPixel(SDL_Surface* surface,
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

void
doPoint(SDL_Surface* surface, const Brush& b, SDL_Point p)
{
  if (b.pen.type == Pen::BOX) {
    int xBeg = p.x - (b.pen.w / 2 + b.pen.w % 2 - 1);
    int xEnd = xBeg + b.pen.w;
    int yBeg = p.y - (b.pen.h / 2 + b.pen.h % 2 - 1);
    int yEnd = yBeg + b.pen.h;
    for (int i = yBeg; i < yEnd; ++i) {
      for (int j = xBeg; j < xEnd; ++j) {
        doPixel(surface, b.pattern.data8x8, {j, i}, b.colorA, b.colorB);
      }
    }
  } else {
    doPixel(surface, b.pattern.data8x8, p, b.colorA, b.colorB);
  }
}

Canvas&
operator|(Canvas& c, SDL_Point p)
{
  doPoint(c.surface, c.brush, p);
  return c;
}

void
doHLine(SDL_Surface* surface, const Brush& b, const drawHorizontalLine& l)
{
  SDL_Point p{l.x, l.y};
  for (int i = 0; i < l.length; ++i, ++p.x) doPoint(surface, b, p);
}

Canvas&
operator|(Canvas& c, drawHorizontalLine l)
{
  if (c.brush.pattern.data8x8 || c.brush.pen.type != Pen::DOT) {
    doHLine(c.surface, c.brush, l);
  } else {
    SDL_Rect rect{l.x, l.y, l.length, 1};
    SDL_FillRect(c.surface, &rect, c.brush.colorA);
  }
  return c;
}

void
doBox(SDL_Surface* surface, const Brush& b, const SDL_Rect& rect)
{
  if (b.pattern.data8x8 || b.pen.type != Pen::DOT) {
    drawHorizontalLine l{rect.x, rect.y, rect.w};
    for (int i = 0; i < rect.h; ++i, ++l.y) doHLine(surface, b, l);
  } else {
    SDL_FillRect(surface, &rect, b.colorA);
  }
}

Canvas&
operator|(Canvas& c, SDL_Rect rect)
{
  doBox(c.surface, c.brush, rect);
  return c;
}

} // namespace pixedit
