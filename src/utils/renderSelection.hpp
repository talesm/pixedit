#ifndef PIXEDIT_SRC_UTILS_RENDER_SELECTION_INCLUDED
#define PIXEDIT_SRC_UTILS_RENDER_SELECTION_INCLUDED

#include <SDL.h>
#include "Canvas.hpp"

namespace pixedit {

constexpr SDL_Color selectedColorA{0, 0, 128, 255};
constexpr SDL_Color selectedColorB{255, 255, 128, 255};

inline void
renderSelection(Canvas& canvas, SDL_Rect rect)
{
  // Backup
  auto bkpColorA = canvas.getRawColorA();
  auto bkpColorB = canvas.getRawColorB();

  canvas | ColorA{selectedColorA} | ColorB{selectedColorB} |
    patterns::CHECKERED_4;

  canvas | OutlineRect{rect};

  // Restore
  canvas | RawColorA{bkpColorA} | RawColorB{bkpColorB} | patterns::SOLID;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_RENDER_SELECTION_INCLUDED */
