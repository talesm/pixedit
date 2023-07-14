#ifndef PIXEDIT_SRC_PRIMITIVES_BLIT_INCLUDED
#define PIXEDIT_SRC_PRIMITIVES_BLIT_INCLUDED

#include "Surface.hpp"
#include "utils/rect.hpp"

namespace pixedit {

struct Blit
{
  Surface surface;
  Point pos;
};

struct BlitScaled
{
  Surface surface;
  Rect rect;
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PRIMITIVES_BLIT_INCLUDED */
