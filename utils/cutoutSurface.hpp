#ifndef PIXEDIT_SRC_UTILS_CUTOUT_SURFACE_INCLUDED
#define PIXEDIT_SRC_UTILS_CUTOUT_SURFACE_INCLUDED

#include <SDL3/SDL.h>
#include "Color.hpp"
#include "Surface.hpp"

namespace pixedit {

inline Surface
copySurface(Surface surface, Rect rect)
{
  if (rect.x >= surface.GetWidth() || rect.y >= surface.GetHeight() ||
      rect.w < 1 || rect.h < 1) {
    return nullptr;
  }
  if (rect.x < 0) {
    rect.w += rect.x;
    rect.x = 0;
  }
  if (rect.y < 0) {
    rect.h += rect.y;
    rect.y = 0;
  }
  if (rect.x + rect.w > surface.GetWidth())
    rect.w -= surface.GetWidth() - rect.x;
  if (rect.y + rect.h > surface.GetHeight())
    rect.h -= surface.GetHeight() - rect.y;

  Surface target = Surface(rect.GetSize(), DEFAULT_FORMAT);
  if (!target) { return nullptr; }
  SDL_BlendMode bkpBlendMode = surface.GetBlendMode();
  surface.SetBlendMode(SDL::BLENDMODE_NONE);
  target.BlitAt(surface, rect, {0, 0});
  surface.SetBlendMode(bkpBlendMode);
  return target;
}

inline Surface
cutoutSurface(Surface surface, const SDL_Rect& rect, Color replaceColor)
{
  if (!surface) { return nullptr; }
  auto cutout = copySurface(surface, rect);
  surface.FillRect(rect, surface.MapRGBA(replaceColor));
  return cutout;
}

inline Surface
cutoutSurface(Surface surface,
              const SDL_Rect& rect,
              Surface mask,
              Color replaceColor)
{
  if (!surface) { return nullptr; }
  auto cutout = copySurface(surface, rect);
  mask.SetColorKey(1);
  cutout.Blit(mask, {}, {});
  mask.SetColorKey(0);
  setColorIndex(mask, 1, replaceColor);
  surface.BlitAt(mask, {}, {rect.x, rect.y});
  return cutout;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_CUTOUT_SURFACE_INCLUDED */
