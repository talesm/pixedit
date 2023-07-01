#ifndef PIXEDIT_SRC_BRUSH_INCLUDED
#define PIXEDIT_SRC_BRUSH_INCLUDED

#include <SDL.h>
#include "Pattern.hpp"
#include "Pen.hpp"

namespace pixedit {

/// @brief The drawing state
struct Brush
{
  Uint32 colorA{};
  Uint32 colorB{};
  Pen pen;
  Pattern pattern{patterns::SOLID};
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_BRUSH_INCLUDED */
