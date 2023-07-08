#ifndef PIXEDIT_SRC_PRIMITIVES_POINT_INCLUDED
#define PIXEDIT_SRC_PRIMITIVES_POINT_INCLUDED

#include <SDL.h>
#include "Canvas.hpp"

namespace pixedit {

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

} // namespace pixedit

#endif /* PIXEDIT_SRC_PRIMITIVES_POINT_INCLUDED */
