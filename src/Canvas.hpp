#ifndef PIXEDIT_SRC_CANVAS_INCLUDED
#define PIXEDIT_SRC_CANVAS_INCLUDED

#include <array>
#include <cstdlib>
#include <SDL.h>
#include "Brush.hpp"
#include "utils/Color.hpp"
#include "utils/Surface.hpp"
#include "utils/safeGetFormat.hpp"

namespace pixedit {

struct HorizontalLine;

struct LineTo;

struct OpenLineTo;

/// @brief A canvas where you can issue drawing commands to.
///
/// See @ref primitives to how to interact with it
class Canvas
{
  Surface surface;
  Brush brush;

public:
  Canvas(Surface surface = {}, bool owning = false)
    : surface(surface)
  {
    if (surface && !owning) { surface.get()->refcount++; }
  }
  Canvas(const Canvas&) = delete;
  Canvas& operator=(const Canvas&) = delete;

  void setSurface(Surface value, bool owning = false);

  constexpr RawColor getRawColorA() const { return brush.colorA; }
  constexpr RawColor getRawColorB() const { return brush.colorB; }

  constexpr Color getColorA() const
  {
    return rawToComponent(brush.colorA, surface.getFormat());
  }
  constexpr Color getColorB() const
  {
    return rawToComponent(brush.colorB, surface.getFormat());
  }

  friend constexpr Canvas& operator|(Canvas& c, RawColor rawColor);
  friend constexpr Canvas& operator|(Canvas& c, RawColorB rawColor);
  friend Canvas& operator|(Canvas& c, Color color);
  friend Canvas& operator|(Canvas& c, ColorB color);
  friend constexpr Canvas& operator|(Canvas& c, Pattern pattern);
  friend constexpr Canvas& operator|(Canvas& c, const Pen& pen);
  friend Canvas& operator|(Canvas& c, SDL_Point p);
  friend Canvas& operator|(Canvas& c, HorizontalLine l);
  friend Canvas& operator|(Canvas& c, LineTo l);
  friend Canvas& operator|(Canvas& c, OpenLineTo l);
  friend Canvas& operator|(Canvas& c, SDL_Rect rect);
};

constexpr Canvas&
operator|(Canvas& c, RawColor rawColor)
{
  c.brush.colorA = rawColor;
  return c;
}

constexpr Canvas&
operator|(Canvas& c, RawColorB rawColor)
{
  c.brush.colorB = rawColor.color;
  return c;
}

/// @brief Set pattern
constexpr Canvas&
operator|(Canvas& c, Pattern pattern)
{
  c.brush.pattern = pattern;
  return c;
}

constexpr Canvas&
operator|(Canvas& c, const Pen& pen)
{
  c.brush.pen = pen;
  return c;
}

struct ColorA : SDL_Color
{
  ColorA(SDL_Color c)
    : SDL_Color(c)
  {
  }
  ColorA(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
    : SDL_Color{r, g, b, a}
  {
  }
};

struct ColorB : ColorA
{
  using ColorA::ColorA;
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_CANVAS_INCLUDED */
