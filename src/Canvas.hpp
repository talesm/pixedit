#ifndef PIXEDIT_SRC_CANVAS_INCLUDED
#define PIXEDIT_SRC_CANVAS_INCLUDED

#include <array>
#include <cstdlib>
#include <SDL.h>
#include "Brush.hpp"
#include "Surface.hpp"
#include "utils/Color.hpp"

namespace pixedit {

// Forward decls
struct HorizontalLine;
struct LineTo;
struct OpenLineTo;
struct Blit;
struct BlitScaled;
struct ColorB;
struct RawColorB;

/// @brief Sets color
using RawColorA = RawColor;

/// @brief A canvas where you can issue drawing commands to.
///
/// See @ref primitives to how to interact with it
class Canvas
{
  Surface surface;
  Brush brush;

public:
  Canvas(Surface surface = {})
    : surface(surface)
  {
  }
  Canvas(const Canvas&) = delete;
  Canvas& operator=(const Canvas&) = delete;

  void setSurface(Surface value);

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

  constexpr const Brush& getBrush() const { return brush; }

  friend constexpr Canvas& operator|(Canvas& c, RawColor rawColor);
  friend constexpr Canvas& operator|(Canvas& c, RawColorB rawColor);
  friend Canvas& operator|(Canvas& c, Color color);
  friend Canvas& operator|(Canvas& c, ColorB color);
  friend constexpr Canvas& operator|(Canvas& c, Pattern pattern);
  friend constexpr Canvas& operator|(Canvas& c, const Pen& pen);
  friend constexpr Canvas& operator|(Canvas& c, const Brush& brush);
  friend Canvas& operator|(Canvas& c, SDL_Point p);
  friend Canvas& operator|(Canvas& c, HorizontalLine l);
  friend Canvas& operator|(Canvas& c, LineTo l);
  friend Canvas& operator|(Canvas& c, OpenLineTo l);
  friend Canvas& operator|(Canvas& c, SDL_Rect rect);
  friend Canvas& operator|(Canvas& c, Blit blit);
  friend Canvas& operator|(Canvas& c, BlitScaled blit);
};

constexpr Canvas&
operator|(Canvas& c, RawColor rawColor)
{
  c.brush.colorA = rawColor;
  return c;
}

/// @brief Sets secondary color
struct RawColorB
{
  RawColor color;
};

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

constexpr Canvas&
operator|(Canvas& c, const Brush& brush)
{
  c.brush = brush;
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
