#ifndef PIXEDIT_SRC_ALGORITHMS_SURFACE_INCLUDED
#define PIXEDIT_SRC_ALGORITHMS_SURFACE_INCLUDED

#include <SDL.h>

namespace pixedit {

/**
 * Get raw pointer to pixel at pos
 *
 * @return the pointer to the pixel or nullptr if out of bounds
 */
constexpr void*
pixelAt(SDL_Surface* surface, int x, int y)
{
  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h) { return nullptr; }
  Uint8* pixelPtr = static_cast<Uint8*>(surface->pixels);
  return pixelPtr + surface->pitch * y + x * surface->format->BytesPerPixel;
}

constexpr void
setPixel(void* pixel, Uint32 value, Uint8 bytesPerPixel)
{
  if (!pixel) return;
  switch (bytesPerPixel) {
  case 1: *static_cast<Uint8*>(pixel) = value; break;
  case 2: *static_cast<Uint16*>(pixel) = value; break;
  case 3: {
    auto pixelPtr = static_cast<Uint8*>(pixel);
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    for (int i = 0; i < 3; i++) { pixelPtr[i] = Uint8(value >> i * 8); }
#else
    for (int i = 0; i < 3; i++) { pixelPtr[i] = Uint8(value >> (2 - i) * 8); }
#endif // SDL_BYTEORDER == SDL_LIL_ENDIAN
    break;
  }
  case 4: *static_cast<Uint32*>(pixel) = value; break;

  default: break;
  }
}

/**
 * Set the pixel value at pos
 *
 * Does nothing if out of bounds. Does not takes alpha nor colorkey in
 * consideration the value should be in the surface's pixel format
 */
constexpr void
setPixelAt(SDL_Surface* surface, int x, int y, Uint32 value)
{
  setPixel(pixelAt(surface, x, y), value, surface->format->BytesPerPixel);
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_ALGORITHMS_SURFACE_INCLUDED */
