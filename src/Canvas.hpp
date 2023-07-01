#ifndef PIXEDIT_SRC_CANVAS_INCLUDED
#define PIXEDIT_SRC_CANVAS_INCLUDED

#include <cstdlib>
#include <SDL.h>
#include "Pattern.hpp"

namespace pixedit {

struct drawHorizontalLine
{
  int x;
  int y;
  int length;
};

struct setColorB;

/**
 * A canvas where you can issue drawing commands to
 */
class Canvas
{
  SDL_Surface* surface;
  Uint32 colorA;
  Uint32 colorB;
  Uint64 pattern = 0;

public:
  constexpr Canvas(SDL_Surface* surface = nullptr)
    : surface(surface)
    , colorA{0}
  {
  }

  void setSurface(SDL_Surface* value);

  constexpr Uint32 getRawColorA() const { return colorA; }
  constexpr Uint32 getRawColorB() const { return colorB; }

  friend constexpr Canvas& operator|(Canvas& c, Uint32 rawColor);
  friend constexpr Canvas& operator|(Canvas& c, setColorB rawColor);
  friend constexpr Canvas& operator|(Canvas& c, Pattern pattern);
  friend Canvas& operator|(Canvas& c, SDL_Point p);
  friend Canvas& operator|(Canvas& c, drawHorizontalLine l);
  friend Canvas& operator|(Canvas& c, SDL_Rect rect);
};

/// @brief Sets color
constexpr Uint32
setColor(Uint32 color)
{
  return {color};
}

/// @brief Sets secondary color
struct setColorB
{
  Uint32 color;
};

/// @brief Set pattern
constexpr Pattern
setPattern(Pattern pattern)
{
  return pattern;
}

/// @{
/// @brief draw point centered at given coordinates
/// @param x the x coordinate
/// @param y the y coordinate
/// @param p the coordinates
/// @return An SDL_Point that might be used with operator| to render the point
constexpr SDL_Point
drawPoint(int x, int y)
{
  return {x, y};
}
constexpr SDL_Point
drawPoint(SDL_Point p)
{
  return p;
}
/// @}

/// @{
/// @brief Fill rect with given position and size
/// @param rect the rect containing the top left position and size
/// @param pos the point containing the top left position
/// @param pos the point containing the size
/// @param x the leftmost pixel
/// @param y the topmost pixel
/// @param w the horizontal size
/// @param h the vertical size
/// @return An SDL_Rect that you can add to canvas through operator|
constexpr SDL_Rect
fillRect(SDL_Rect rect)
{
  return rect;
}
constexpr auto
fillRect(int x, int y, int w, int h)
{
  return fillRect({x, y, w, h});
}
constexpr auto
fillRect(SDL_Point p, SDL_Point sz)
{
  return fillRect(p.x, p.y, sz.x, sz.y);
}
/// @}

/// @{
/// @brief Fill rect from (x1,y1) to (x2, y2), inclusive
/// @param x1 the first x position
/// @param y1 the first y position
/// @param x2 the second x position
/// @param y2 the second y position
/// @param p1 the first position
/// @param p2 the second poistion
/// @return the rectangle to fill (to be used on Canvas in combination with
/// operator|)
constexpr auto
fillRectTo(int x1, int y1, int x2, int y2)
{
  using std::min, std::abs;
  return fillRect(min(x1, x2), min(y1, y2), abs(x2 - x1) + 1, abs(y2 - y1) + 1);
}
constexpr auto
fillRectTo(SDL_Point p1, SDL_Point p2)
{
  return fillRectTo(p1.x, p1.y, p2.x, p2.y);
}
/// @}

/// @brief Represents an rect outlined (not filled)
struct OutlineRect
{
  SDL_Rect rect;
};
/// @{
/// @brief Draw rect outline with given position and size
/// @param rect the rect containing the top left position and size
/// @param pos the point containing the top left position
/// @param pos the point containing the size
/// @param x the leftmost pixel
/// @param y the topmost pixel
/// @param w the horizontal size
/// @param h the vertical size
/// @return An OutlineRect that you can add to canvas through operator|
constexpr OutlineRect
drawRect(SDL_Rect rect)
{
  return {rect};
}
constexpr auto
drawRect(int x, int y, int w, int h)
{
  return drawRect({x, y, w, h});
}
constexpr auto
drawRect(SDL_Point p, SDL_Point sz)
{
  return drawRect(p.x, p.y, sz.x, sz.y);
}
/// @}

/// @{
/// @brief Draw rect outline from (x1,y1) to (x2, y2), inclusive
/// @param x1 the first x position
/// @param y1 the first y position
/// @param x2 the second x position
/// @param y2 the second y position
/// @param p1 the first position
/// @param p2 the second poistion
/// @return the outline rectangle to draw (to be used on Canvas in combination
/// with operator|)
constexpr auto
drawRectTo(int x1, int y1, int x2, int y2)
{
  using std::min, std::abs;
  return drawRect(min(x1, x2), min(y1, y2), abs(x2 - x1) + 1, abs(y2 - y1) + 1);
}
constexpr auto
drawRectTo(SDL_Point p1, SDL_Point p2)
{
  return drawRectTo(p1.x, p1.y, p2.x, p2.y);
}
/// @}

struct drawVerticalLine
{
  int x;
  int y;
  int length;
};

constexpr Canvas&
operator|(Canvas& c, Uint32 rawColor)
{
  c.colorA = rawColor;
  return c;
}

constexpr Canvas&
operator|(Canvas& c, setColorB rawColor)
{
  c.colorB = rawColor.color;
  return c;
}

constexpr Canvas&
operator|(Canvas& c, Pattern pattern)
{
  c.pattern = pattern.data8x8;
  return c;
}

inline Canvas&
operator|(Canvas& c, drawVerticalLine l)
{
  return c | fillRect(l.x, l.y, 1, l.length);
}

inline Canvas&
operator|(Canvas& c, OutlineRect outline)
{
  if (outline.rect.h <= 2) { return c | fillRect(outline.rect); }
  int x1 = outline.rect.x, y1 = outline.rect.y;
  int w = outline.rect.w, h = outline.rect.h;
  int x2 = x1 + w - 1, y2 = y1 + h - 1;
  return c | drawHorizontalLine{x1, y1, w} | drawHorizontalLine{x1, y2, w} |
         drawVerticalLine{x1, y1 + 1, h - 2} |
         drawVerticalLine{x2, y1 + 1, h - 2};
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_CANVAS_INCLUDED */
