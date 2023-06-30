#ifndef PIXEDIT_SRC_CANVAS_INCLUDED
#define PIXEDIT_SRC_CANVAS_INCLUDED

#include <SDL.h>

namespace pixedit {

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

constexpr Canvas&
operator|(Canvas& c, Uint32 rawColor)
{
  c.setPrimaryColorRaw(rawColor);
  return c;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_CANVAS_INCLUDED */
