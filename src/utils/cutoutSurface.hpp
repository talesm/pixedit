#ifndef PIXEDIT_SRC_UTILS_CUTOUT_SURFACE_INCLUDED
#define PIXEDIT_SRC_UTILS_CUTOUT_SURFACE_INCLUDED

#include <SDL.h>
#include "Surface.hpp"

namespace pixedit {

inline Surface
copySurface(Surface surface, SDL_Rect rect)
{
  if (rect.x >= surface.getW() || rect.y >= surface.getH() || rect.w < 1 ||
      rect.h < 1)
    return nullptr;
  if (rect.x < 0) {
    rect.w += rect.x;
    rect.x = 0;
  }
  if (rect.y < 0) {
    rect.h += rect.y;
    rect.y = 0;
  }
  if (rect.x + rect.w > surface.getW()) rect.w -= surface.getW() - rect.x;
  if (rect.y + rect.h > surface.getH()) rect.h -= surface.getH() - rect.y;

  Surface target{SDL_CreateRGBSurfaceWithFormat(
                   0, rect.w, rect.h, 32, SDL_PIXELFORMAT_ABGR32),
                 true};
  if (!target) return nullptr;
  SDL_BlendMode bkpBlendMode = surface.getBlendMode();
  surface.setBlendMode(SDL_BLENDMODE_NONE);
  target.blit(surface, {0, 0}, rect);
  surface.setBlendMode(bkpBlendMode);
  return target;
}

inline Surface
cutoutSurface(Surface surface, const SDL_Rect& rect, Uint32 replaceColor)
{
  if (!surface) return nullptr;
  auto cutout = copySurface(surface, rect);
  surface.fillRect(rect, replaceColor);
  return cutout;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_CUTOUT_SURFACE_INCLUDED */
