#include "Canvas.hpp"
#include "primitives/Line.hpp"
#include "utils/pixel.hpp"
#include "utils/rasterLine.hpp"

namespace pixedit {

void
Canvas::setSurface(SDL_Surface* value, bool owning)
{
  if (surface == value) return;
  brush.colorA = componentToRaw(
    rawToComponent(brush.colorA, safeGetFormat(surface)), safeGetFormat(value));
  brush.colorB = componentToRaw(
    rawToComponent(brush.colorB, safeGetFormat(surface)), safeGetFormat(value));
  SDL_FreeSurface(surface);
  surface = value;
  if (surface && !owning) { surface->refcount++; }
}

Canvas&
operator|(Canvas& c, Color color)
{
  c.brush.colorA =
    componentToRaw(color, c.surface ? c.surface->format : nullptr);
  return c;
}
Canvas&
operator|(Canvas& c, ColorB color)
{
  c.brush.colorB =
    componentToRaw(color, c.surface ? c.surface->format : nullptr);
  return c;
}

void
doPixel(SDL_Surface* surface, const Brush& b, SDL_Point p)
{
  auto color = b.colorA;
  if (b.pattern.data8x8) {
    int xx = p.x % 8;
    int yy = p.y % 8;
    if ((b.pattern.data8x8 >> (yy * 8 + xx)) & 1) { color = b.colorB; }
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
      for (int j = xBeg; j < xEnd; ++j) { doPixel(surface, b, {j, i}); }
    }
  } else {
    doPixel(surface, b, p);
  }
}

Canvas&
operator|(Canvas& c, SDL_Point p)
{
  doPoint(c.surface, c.brush, p);
  return c;
}

void
doHLine(SDL_Surface* surface, const Brush& b, const HorizontalLine& l)
{
  SDL_Point p{l.x, l.y};
  for (int i = 0; i < l.length; ++i, ++p.x) doPoint(surface, b, p);
}

Canvas&
operator|(Canvas& c, HorizontalLine l)
{
  if (c.brush.pattern.data8x8 || c.brush.pen.type != Pen::DOT) {
    doHLine(c.surface, c.brush, l);
  } else {
    SDL_Rect rect{l.x, l.y, l.length, 1};
    SDL_FillRect(c.surface, &rect, c.brush.colorA);
  }
  return c;
}

Canvas&
operator|(Canvas& c, LineTo l)
{
  rasterLine(l.x1, l.y1, l.x2, l.y2, [&](int x, int y) {
    doPoint(c.surface, c.brush, {x, y});
  });
  return c;
}

Canvas&
operator|(Canvas& c, OpenLineTo l)
{
  rasterLineOpen(l.x1, l.y1, l.x2, l.y2, [&](int x, int y) {
    doPoint(c.surface, c.brush, {x, y});
  });
  return c;
}

void
doBox(SDL_Surface* surface, const Brush& b, const SDL_Rect& rect)
{
  if (b.pattern.data8x8 || b.pen.type != Pen::DOT) {
    HorizontalLine l{rect.x, rect.y, rect.w};
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
