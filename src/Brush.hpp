#ifndef PIXEDIT_SRC_BRUSH_INCLUDED
#define PIXEDIT_SRC_BRUSH_INCLUDED

#include <SDL.h>
#include "Pattern.hpp"
#include "Pen.hpp"

namespace pixedit {

/// @brief Raw color type
using RawColor = Uint32;

/// @brief The drawing state
struct Brush
{
  RawColor colorA{};
  RawColor colorB{};
  Pen pen;
  Pattern pattern{patterns::SOLID};
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_BRUSH_INCLUDED */
