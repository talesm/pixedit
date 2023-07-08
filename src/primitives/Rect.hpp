#ifndef PIXEDIT_SRC_PRIMITIVES_RECT_INCLUDED
#define PIXEDIT_SRC_PRIMITIVES_RECT_INCLUDED

#include <SDL.h>
#include "Canvas.hpp"
#include "Line.hpp"

namespace pixedit {

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

inline Canvas&
operator|(Canvas& c, OutlineRect outline)
{
  if (outline.h <= 2) { return c | FillRect(outline); }
  int x1 = outline.x, y1 = outline.y;
  int w = outline.w, h = outline.h;
  int x2 = x1 + w - 1, y2 = y1 + h - 1;
  return c | HorizontalLine{x1, y1, w} | HorizontalLine{x1, y2, w} |
         VerticalLine{x1, y1 + 1, h - 2} | VerticalLine{x2, y1 + 1, h - 2};
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_PRIMITIVES_RECT_INCLUDED */
