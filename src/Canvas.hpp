#ifndef PIXEDIT_SRC_CANVAS_INCLUDED
#define PIXEDIT_SRC_CANVAS_INCLUDED

#include <cstdlib>
#include <SDL.h>
#include "Brush.hpp"

namespace pixedit {

struct HorizontalLine
{
  int x;
  int y;
  int length;
};

struct LineTo
{
  int x1, y1, x2, y2;
  LineTo(int x1, int y1, int x2, int y2)
    : x1(x1)
    , y1(y1)
    , x2(x2)
    , y2(y2)
  {
  }
  LineTo(const SDL_Point& p1, const SDL_Point& p2)
    : LineTo(p1.x, p1.y, p2.x, p2.y)
  {
  }
};

struct OpenLineTo : LineTo
{
  using LineTo::LineTo;
};

/// @brief Sets color
using RawColorA = RawColor;

/// @brief Sets secondary color
struct RawColorB
{
  RawColor color;
};

/**
 * A canvas where you can issue drawing commands to
 */
class Canvas
{
  SDL_Surface* surface;
  Brush brush;

public:
  constexpr Canvas(SDL_Surface* surface = nullptr)
    : surface(surface)
  {
  }

  void setSurface(SDL_Surface* value);

  constexpr RawColor getRawColorA() const { return brush.colorA; }
  constexpr RawColor getRawColorB() const { return brush.colorB; }

  friend constexpr Canvas& operator|(Canvas& c, RawColor rawColor);
  friend constexpr Canvas& operator|(Canvas& c, RawColorB rawColor);
  friend constexpr Canvas& operator|(Canvas& c, Pattern pattern);
  friend constexpr Canvas& operator|(Canvas& c, const Pen& pen);
  friend Canvas& operator|(Canvas& c, SDL_Point p);
  friend Canvas& operator|(Canvas& c, HorizontalLine l);
  friend Canvas& operator|(Canvas& c, LineTo l);
  friend Canvas& operator|(Canvas& c, OpenLineTo l);
  friend Canvas& operator|(Canvas& c, SDL_Rect rect);
};

/// @{
/// @brief draw point centered at given coordinates
/// @param x the x coordinate
/// @param y the y coordinate
/// @param p the coordinates
/// @return An SDL_Point that might be used with operator| to render the point
constexpr SDL_Point
Point(int x, int y)
{
  return {x, y};
}
constexpr SDL_Point
Point(SDL_Point p)
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
FillRect(SDL_Rect rect)
{
  return rect;
}
constexpr auto
FillRect(int x, int y, int w, int h)
{
  return FillRect({x, y, w, h});
}
constexpr auto
FillRect(SDL_Point p, SDL_Point sz)
{
  return FillRect(p.x, p.y, sz.x, sz.y);
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
FillRectTo(int x1, int y1, int x2, int y2)
{
  using std::min, std::abs;
  return FillRect(min(x1, x2), min(y1, y2), abs(x2 - x1) + 1, abs(y2 - y1) + 1);
}
constexpr auto
FillRectTo(SDL_Point p1, SDL_Point p2)
{
  return FillRectTo(p1.x, p1.y, p2.x, p2.y);
}
/// @}

/// @brief Represents an rect outlined (not filled)
struct OutlineRect : SDL_Rect
{
  /// @{
  /// @brief Constructs outline rect command with given position and size
  /// @param rect the rect containing the top left position and size
  /// @param pos the point containing the top left position
  /// @param pos the point containing the size
  /// @param x the leftmost pixel
  /// @param y the topmost pixel
  /// @param w the horizontal size
  /// @param h the vertical size
  /// @return An OutlineRect that you can add to canvas through operator|
  constexpr OutlineRect(SDL_Rect rect)
    : SDL_Rect(rect)
  {
  }
  constexpr OutlineRect(int x, int y, int w, int h)
    : OutlineRect(SDL_Rect{x, y, w, h})
  {
  }
  constexpr OutlineRect(SDL_Point p, SDL_Point sz)
    : OutlineRect(p.x, p.y, sz.x, sz.y)
  {
  }
  /// @}
};

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
OutlineRectTo(int x1, int y1, int x2, int y2)
{
  using std::min, std::abs;
  return OutlineRect(
    min(x1, x2), min(y1, y2), abs(x2 - x1) + 1, abs(y2 - y1) + 1);
}
constexpr auto
OutlineRectTo(SDL_Point p1, SDL_Point p2)
{
  return OutlineRectTo(p1.x, p1.y, p2.x, p2.y);
}
/// @}

struct drawVerticalLine
{
  int x;
  int y;
  int length;
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

inline Canvas&
operator|(Canvas& c, drawVerticalLine l)
{
  return c | FillRect(l.x, l.y, 1, l.length);
}

inline Canvas&
operator|(Canvas& c, OutlineRect outline)
{
  if (outline.h <= 2) { return c | FillRect(outline); }
  int x1 = outline.x, y1 = outline.y;
  int w = outline.w, h = outline.h;
  int x2 = x1 + w - 1, y2 = y1 + h - 1;
  return c | HorizontalLine{x1, y1, w} | HorizontalLine{x1, y2, w} |
         drawVerticalLine{x1, y1 + 1, h - 2} |
         drawVerticalLine{x2, y1 + 1, h - 2};
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_CANVAS_INCLUDED */
