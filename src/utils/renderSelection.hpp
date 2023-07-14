#ifndef PIXEDIT_SRC_UTILS_RENDER_SELECTION_INCLUDED
#define PIXEDIT_SRC_UTILS_RENDER_SELECTION_INCLUDED

#include <vector>
#include <SDL.h>
#include "Canvas.hpp"
#include "primitives/Blit.hpp"
#include "primitives/Poly.hpp"
#include "primitives/Rect.hpp"

namespace pixedit {

constexpr SDL_Color selectedColorA{0, 0, 128, 255};
constexpr SDL_Color selectedColorB{255, 255, 128, 255};

inline void
renderSelection(Canvas& canvas, SDL_Rect rect, bool inProgress = false)
{
  // Backup
  auto bkp = canvas.getBrush();

  canvas | Pen{} | ColorA{selectedColorA} | ColorB{selectedColorB};
  canvas | (inProgress ? patterns::CHECKERED_2 : patterns::CHECKERED_4);

  canvas | OutlineRect{rect};

  // Restore
  canvas | bkp;
}

constexpr SDL_Color freeSelectedColorA{128, 0, 0, 255};
constexpr SDL_Color freeSelectedColorB{128, 255, 255, 255};

inline void
renderSelection(Canvas& canvas,
                const std::vector<SDL_Point>& points,
                bool inProgress = false)
{
  // Backup
  auto bkp = canvas.getBrush();

  canvas | Pen{} | ColorA{freeSelectedColorA} | ColorB{freeSelectedColorB};
  canvas | (inProgress ? patterns::CHECKERED_2 : patterns::CHECKERED_4);

  if (inProgress) {
    canvas | Lines{points};
  } else {
    canvas | OutlinePoly{points};
  }

  // Restore
  canvas | bkp;
}

inline void
contour(Canvas& canvas, Surface mask, SDL_Point offset = {0, 0})
{
  constexpr static SDL_Point borders[] = {
    {-1, -1},
    {0, -1},
    {1, -1},
    {1, 0},
    {1, 1},
    {0, 1},
    {-1, 1},
    {-1, 0},
  };
  for (int y = 0; y < mask.getH(); ++y) {
    for (int x = 0; x < mask.getW(); ++x) {
      if (!mask.getPixel(x, y)) { continue; }
      bool isCentral = true;
      for (auto&& b : borders) {
        if (!mask.getPixel(x + b.x, y + b.y)) {
          isCentral = false;
          break;
        }
      }
      if (isCentral) { continue; }
      canvas | Point(x + offset.x, y + offset.y);
    }
  }
}

inline void
renderSelection(Canvas& canvas,
                SDL_Rect rect,
                Surface mask,
                bool inProgress = false)
{
  if (!mask) {
    renderSelection(canvas, rect, inProgress);
    return;
  }

  // Backup
  auto bkp = canvas.getBrush();

  canvas | Pen{};

  if (!inProgress) {
    canvas | ColorA{freeSelectedColorA} | ColorB{freeSelectedColorB};
    canvas | (inProgress ? patterns::CHECKERED_2 : patterns::CHECKERED_4);
    canvas | OutlineRect{rect};
  }

  if (mask.getW() == rect.w && mask.getH() == rect.h) {
    if (inProgress) {
      canvas | ColorA{freeSelectedColorA} | ColorB{freeSelectedColorB};
    } else {
      canvas | ColorA{selectedColorA} | ColorB{selectedColorB};
    }
    canvas | patterns::CHECKERED_2;
    contour(canvas, mask, {rect.x, rect.y});
  } else {
    Surface temp = Surface::create(mask.getW(), mask.getH());
    Canvas c{temp};
    if (inProgress) {
      c | ColorA{freeSelectedColorA} | ColorB{freeSelectedColorB};
    } else {
      c | ColorA{selectedColorA} | ColorB{selectedColorB};
    }
    c | patterns::CHECKERED_2;
    contour(c, mask);
    canvas | BlitScaled{temp, rect};
  }
  // Restore
  canvas | bkp;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_RENDER_SELECTION_INCLUDED */
