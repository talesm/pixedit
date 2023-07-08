#ifndef PIXEDIT_SRC_PRIMITIVES_LINES_INCLUDED
#define PIXEDIT_SRC_PRIMITIVES_LINES_INCLUDED

#include <span>
#include "Canvas.hpp"
#include "Line.hpp"
#include "Point.hpp"

namespace pixedit {

using OpenLines = std::span<const SDL_Point>;

struct Lines
{
  const std::span<const SDL_Point> vertices;

  Lines(std::span<const SDL_Point> vertices)
    : vertices(vertices)
  {
  }
};

inline Canvas&
operator|(Canvas& c, OpenLines lns)
{
  for (auto it = lns.begin() + 1; it != lns.end(); ++it) {
    c | OpenLineTo(*(it - 1), *it);
  }
  return c;
}

inline Canvas&
operator|(Canvas& c, Lines lns)
{
  return c | lns.vertices | lns.vertices.back();
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_PRIMITIVES_LINES_INCLUDED */
