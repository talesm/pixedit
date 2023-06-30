#ifndef PIXEDIT_SRC_CANVAS_INCLUDED
#define PIXEDIT_SRC_CANVAS_INCLUDED

#include <cstdlib>
#include <SDL.h>

namespace pixedit {

struct HorizontalLine
{
  int x;
  int y;
  int length;
};

/**
 * A canvas where you can issue drawing commands to
 */
class Canvas
{
  SDL_Surface* surface;
  Uint32 pColor;

public:
  constexpr Canvas(SDL_Surface* surface = nullptr)
    : surface(surface)
    , pColor{0}
  {
  }

  void setSurface(SDL_Surface* value);

  constexpr void setPrimaryColorRaw(Uint32 value) { pColor = value; }

  constexpr Uint32 getPrimaryColorRaw() const { return pColor; }

  friend Canvas& operator|(Canvas& c, SDL_Point p);
  friend Canvas& operator|(Canvas& c, HorizontalLine l);
  friend Canvas& operator|(Canvas& c, SDL_Rect rect);
};

/// @brief Sets color
/// @param color
/// @return
constexpr Uint32
setColor(Uint32 color)
{
  return {color};
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

inline Canvas&
operator|(Canvas& c, Uint32 rawColor)
{
  c.setPrimaryColorRaw(rawColor);
  return c;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_CANVAS_INCLUDED */
