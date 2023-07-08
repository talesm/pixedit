#ifndef PIXEDIT_SRC_PRIMITIVES_POLY_INCLUDED
#define PIXEDIT_SRC_PRIMITIVES_POLY_INCLUDED

#include "Canvas.hpp"
#include "Line.hpp"
#include "Lines.hpp"
#include "utils/rasterPoly.hpp"

namespace pixedit {

struct OutlinePoly : Lines
{
  using Lines::Lines;
};

inline Canvas&
operator|(Canvas& c, OutlinePoly lns)
{
  if (lns.vertices.size() < 3) { return c | static_cast<Lines>(lns); }
  return c | lns.vertices |
         OpenLineTo(lns.vertices.back(), lns.vertices.front());
}

struct FillPoly : Lines
{
  using Lines::Lines;
};

inline Canvas&
operator|(Canvas& c, FillPoly lns)
{
  if (lns.vertices.size() < 3) { return c | static_cast<Lines>(lns); }
  rasterPoly(lns.vertices, [&](int x, int y, int len) {
    c | HorizontalLine{x, y, len};
  });
  return c;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_PRIMITIVES_POLY_INCLUDED */
