#ifndef PIXEDIT_SRC_PRIMITIVES_OVAL_INCLUDED
#define PIXEDIT_SRC_PRIMITIVES_OVAL_INCLUDED

#include "Canvas.hpp"
#include "Line.hpp"
#include "Point.hpp"
#include "utils/rasterOval.hpp"

namespace pixedit {

/// @brief Represents an filled oval
struct FillOval : SDL_Rect
{
  /// @{
  /// @brief Constructs oval command with given position and size
  /// @param rect the rect containing the top left position and size
  /// @param pos the point containing the top left position
  /// @param pos the point containing the size
  /// @param x the leftmost pixel
  /// @param y the topmost pixel
  /// @param w the horizontal size
  /// @param h the vertical size
  /// @return An FillOval that you can add to canvas through operator|
  constexpr FillOval(SDL_Rect rect)
    : SDL_Rect(rect)
  {
  }
  constexpr FillOval(int x, int y, int w, int h)
    : FillOval(SDL_Rect{x, y, w, h})
  {
  }
  constexpr FillOval(SDL_Point p, SDL_Point sz)
    : FillOval(p.x, p.y, sz.x, sz.y)
  {
  }
  /// @}
};

/// @{
/// @brief Fill oval from (x1,y1) to (x2, y2), inclusive
/// @param x1 the first x position
/// @param y1 the first y position
/// @param x2 the second x position
/// @param y2 the second y position
/// @param p1 the first position
/// @param p2 the second position
/// @return the ellipse to fill (to be used on Canvas in combination with
/// operator|)
constexpr auto
FillOvalTo(int x1, int y1, int x2, int y2)
{
  using std::min, std::abs;
  return FillOval(min(x1, x2), min(y1, y2), abs(x2 - x1) + 1, abs(y2 - y1) + 1);
}
constexpr auto
FillOvalTo(SDL_Point p1, SDL_Point p2)
{
  return FillOvalTo(p1.x, p1.y, p2.x, p2.y);
}
/// @}

/// @brief Represents an rect outlined (not filled)
struct OutlineOval : SDL_Rect
{
  /// @{
  /// @brief Constructs outline oval command with given position and size
  /// @param rect the rect containing the top left position and size
  /// @param pos the point containing the top left position
  /// @param pos the point containing the size
  /// @param x the leftmost pixel
  /// @param y the topmost pixel
  /// @param w the horizontal size
  /// @param h the vertical size
  /// @return An OutlineOval that you can add to canvas through operator|
  constexpr OutlineOval(SDL_Rect rect)
    : SDL_Rect(rect)
  {
  }
  constexpr OutlineOval(int x, int y, int w, int h)
    : OutlineOval(SDL_Rect{x, y, w, h})
  {
  }
  constexpr OutlineOval(SDL_Point p, SDL_Point sz)
    : OutlineOval(p.x, p.y, sz.x, sz.y)
  {
  }
  /// @}
};

/// @{
/// @brief Draw oval outline from (x1,y1) to (x2, y2), inclusive
/// @param x1 the first x position
/// @param y1 the first y position
/// @param x2 the second x position
/// @param y2 the second y position
/// @param p1 the first position
/// @param p2 the second position
/// @return the outline ellipse to draw (to be used on Canvas in combination
/// with operator|)
constexpr auto
OutlineOvalTo(int x1, int y1, int x2, int y2)
{
  using std::min, std::abs;
  return OutlineOval(
    min(x1, x2), min(y1, y2), abs(x2 - x1) + 1, abs(y2 - y1) + 1);
}
constexpr auto
OutlineOvalTo(SDL_Point p1, SDL_Point p2)
{
  return OutlineOvalTo(p1.x, p1.y, p2.x, p2.y);
}
/// @}

inline Canvas&
operator|(Canvas& c, FillOval oval)
{
  rasterOval(oval.x, oval.y, oval.w, oval.h, [&](int x, int y, int d) {
    c | HorizontalLine{x, y, d + 1};
  });
  return c;
}

inline Canvas&
operator|(Canvas& c, OutlineOval oval)
{
  rasterOvalOutline(
    oval.x, oval.y, oval.w, oval.h, [&](int x, int y) { c | Point(x, y); });
  return c;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_PRIMITIVES_OVAL_INCLUDED */
