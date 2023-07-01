#ifndef PIXEDIT_SRC_PEN_INCLUDED
#define PIXEDIT_SRC_PEN_INCLUDED

#include <array>
#include <cassert>
#include <SDL.h>

namespace pixedit {

struct Pen
{
  enum Type
  {
    DOT,
    BOX,
  };
  Type type;
  int w, h;

  constexpr Pen()
    : type(DOT)
    , w(1)
    , h(1)
  {
  }
  constexpr Pen(int w, int h)
    : type((w == 1 && h == 1) ? DOT : BOX)
    , w(w)
    , h(h)
  {
    assert(w > 0 && h > 0);
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PEN_INCLUDED */
