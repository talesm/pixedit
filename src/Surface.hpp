#ifndef PIXEDIT_SRC_SURFACE_INCLUDED
#define PIXEDIT_SRC_SURFACE_INCLUDED

#include <optional>
#include <string>
#include <SDL3/SDL.h>
#include "utils/Color.hpp"
#include "utils/pixel.hpp"
#include "utils/rect.hpp"
#include "utils/safeGetFormat.hpp"

namespace pixedit {

/// @brief Wraps around surface
class Surface
{
  SDL_Surface* surface;

public:
  constexpr Surface()
    : surface(nullptr){};
  constexpr Surface(nullptr_t)
    : surface(nullptr){};
  constexpr Surface(SDL_Surface* surface, bool owning)
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

  Surface clone() const;

  Surface cloneWith(SDL_PixelFormat format) const;

  Surface cloneWith(const SDL_PixelFormat* format) const;

  static constexpr SDL_PixelFormat DEFAULT_FORMAT = SDL_PIXELFORMAT_ABGR32;

  static Surface create(int w, int h)
  {
    return {
      SDL_CreateSurface(w, h, DEFAULT_FORMAT),
      true,
    };
  }

  static Surface createMask(int w, int h)
  {
    auto surface = SDL_CreateSurface(w, h, DEFAULT_FORMAT);
    auto palette = SDL_CreateSurfacePalette(surface);
    static SDL_Color colors[2] = {{0, 0, 0, 0}, {255, 255, 255, 0}};
    SDL_SetPaletteColors(palette, colors, 0, 2);
    return {surface, true};
  }

  const SDL_PixelFormatDetails* getFormat() const
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

  constexpr Point getSize() const { return {getW(), getH()}; }

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
    SDL_FillSurfaceRect(surface, &rect, color);
  }

  void reset()
  {
    SDL_DestroySurface(surface);
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
    return SDL_BlitSurfaceScaled(rhs.surface, nullptr, surface, &rect, SDL_SCALEMODE_NEAREST);
  }

  int blitScaled(const Surface& rhs, Rect rect, const Rect& subRect)
  {
    return SDL_BlitSurfaceScaled(rhs.surface, &subRect, surface, &rect, SDL_SCALEMODE_NEAREST);
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

  bool save(const std::string& filename) const;

  static Surface load(const std::string& filename);

  std::optional<Uint32> getColorKey() const
  {
    Uint32 key;
    if (SDL_GetSurfaceColorKey(surface, &key) == 0) { return key; }
    return {};
  }
  void setColorKey(Uint32 color) { SDL_SetSurfaceColorKey(surface, true, color); }
  void setColorKey(Color color)
  {
    if (!surface) return;
    auto format = SDL_GetPixelFormatDetails(surface->format);
    setColorKey(
      SDL_MapRGBA(format, nullptr, color.r, color.g, color.b, color.a));
  }
  void unsetColorKey() { SDL_SetSurfaceColorKey(surface, false, 0); }

  void setColorIndex(int index, SDL_Color color)
  {
    SDL_SetPaletteColors(SDL_GetSurfacePalette(surface), &color, index, 1);
  }

  Uint32 mapColor(Color c) const
  {
    auto format = SDL_GetPixelFormatDetails(surface->format);
    return SDL_MapRGBA(format, nullptr, c.r, c.g, c.b, c.a);
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_SURFACE_INCLUDED */
