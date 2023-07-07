#ifndef PIXEDIT_SRC_UTILS_CUTOUT_SURFACE_INCLUDED
#define PIXEDIT_SRC_UTILS_CUTOUT_SURFACE_INCLUDED

#include <SDL.h>

namespace pixedit {

inline SDL_Surface*
copySurface(SDL_Surface* surface, SDL_Rect rect)
{
  if (rect.x >= surface->w || rect.y >= surface->h || rect.w < 1 || rect.h < 1)
    return nullptr;
  if (rect.x < 0) {
    rect.w += rect.x;
    rect.x = 0;
  }
  if (rect.y < 0) {
    rect.h += rect.y;
    rect.y = 0;
  }
  if (rect.x + rect.w > surface->w) rect.w -= surface->w - rect.x;
  if (rect.y + rect.h > surface->h) rect.h -= surface->h - rect.y;

  SDL_Surface* target = SDL_CreateRGBSurfaceWithFormat(
    0, rect.w, rect.h, 32, SDL_PIXELFORMAT_ABGR32);
  if (!target) return nullptr;
  SDL_BlendMode bkpBlendMode;
  SDL_GetSurfaceBlendMode(surface, &bkpBlendMode);
  SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
  SDL_BlitSurface(surface, &rect, target, nullptr);
  SDL_SetSurfaceBlendMode(surface, bkpBlendMode);
  return target;
}

inline SDL_Surface*
cutoutSurface(SDL_Surface* surface, const SDL_Rect& rect, Uint32 replaceColor)
{
  if (!surface) return nullptr;
  auto cutout = copySurface(surface, rect);
  SDL_FillRect(surface, &rect, replaceColor);
  return cutout;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_CUTOUT_SURFACE_INCLUDED */
