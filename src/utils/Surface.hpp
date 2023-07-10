#ifndef PIXEDIT_SRC_UTILS_SURFACE_INCLUDED
#define PIXEDIT_SRC_UTILS_SURFACE_INCLUDED

#include <SDL.h>
#include "pixel.hpp"
#include "rect.hpp"
#include "safeGetFormat.hpp"

namespace pixedit {

/// @brief Wraps around surface
class Surface
{
  SDL_Surface* surface;

public:
  constexpr Surface()
    : surface(nullptr){};
  constexpr Surface(SDL_Surface* surface, bool owning = false)
    : surface(surface)
  {
    if (surface && !owning) { surface->refcount++; }
  }
  ~Surface() { reset(); };
  constexpr Surface(const Surface& rhs)
    : surface(rhs.surface)
  {
    if (rhs.surface) rhs.surface->refcount++;
  }
  constexpr Surface(Surface&& rhs)
    : surface(rhs.surface)
  {
    rhs.surface = nullptr;
  }

  Surface& operator=(Surface rhs)
  {
    std::swap(surface, rhs.surface);
    return *this;
  }

  constexpr SDL_PixelFormat* getFormat() const
  {
    return safeGetFormat(surface);
  }

  constexpr int getW() const
  {
    if (!surface) return 0;
    return surface->w;
  }

  constexpr int getH() const
  {
    if (!surface) return 0;
    return surface->h;
  }

  constexpr void* pixel(int x, int y) const
  {
    if (!surface) return nullptr;
    return pixelAt(surface, x, y);
  }

  constexpr Uint32 getPixel(int x, int y) const
  {
    if (!surface) return 0;
    return getPixelAt(surface, x, y);
  }

  constexpr void setPixel(int x, int y, Uint32 color)
  {
    if (!surface) return;
    setPixelAt(surface, x, y, color);
  }

  void fillRect(const Rect& rect, Uint32 color)
  {
    if (!surface) return;
    SDL_FillRect(surface, &rect, color);
  }

  void reset()
  {
    SDL_FreeSurface(surface);
    surface = nullptr;
  }

  operator bool() const { return surface; }

  constexpr SDL_Surface* get() const { return surface; }
  constexpr SDL_Surface* release()
  {
    auto s = surface;
    surface = nullptr;
    return s;
  }

  int blit(const Surface& rhs, const SDL_Point& p = {})
  {
    SDL_Rect rect{p.x, p.y, 0, 0};
    return SDL_BlitSurface(rhs.surface, nullptr, surface, &rect);
  }

  int blit(const Surface& rhs, const SDL_Point& p, const Rect& subRect)
  {
    SDL_Rect rect{p.x, p.y, 0, 0};
    return SDL_BlitSurface(rhs.surface, &subRect, surface, &rect);
  }

  int blitScaled(const Surface& rhs, Rect rect)
  {
    return SDL_BlitScaled(rhs.surface, nullptr, surface, &rect);
  }

  int blitScaled(const Surface& rhs, Rect rect, const Rect& subRect)
  {
    return SDL_BlitScaled(rhs.surface, &subRect, surface, &rect);
  }

  SDL_BlendMode getBlendMode() const
  {
    SDL_BlendMode mode{};
    SDL_GetSurfaceBlendMode(surface, &mode);
    return mode;
  }
  void setBlendMode(SDL_BlendMode mode)
  {
    SDL_SetSurfaceBlendMode(surface, mode);
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_SURFACE_INCLUDED */
