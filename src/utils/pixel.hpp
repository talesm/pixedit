#ifndef PIXEDIT_SRC_UTILS_PIXEL_INCLUDED
#define PIXEDIT_SRC_UTILS_PIXEL_INCLUDED

#include <SDL3/SDL.h>

namespace pixedit {

/**
 * Get raw pointer to pixel at pos
 *
 * @return the pointer to the pixel or nullptr if out of bounds
 */
constexpr void*
pixelAt(Surface& surface, int x, int y)
{
  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h) { return nullptr; }
  auto pixelPtr = static_cast<Uint8*>(surface->pixels);
  return pixelPtr + surface->pitch * y +
         x * surface.GetFormat().GetBytesPerPixel();
}
constexpr const void*
pixelAt(const Surface& surface, int x, int y)
{
  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h) { return nullptr; }
  auto pixelPtr = static_cast<Uint8*>(surface->pixels);
  return pixelPtr + surface->pitch * y +
         x * surface.GetFormat().GetBytesPerPixel();
}

constexpr Uint32
getPixel(const void* pixel, Uint8 bytesPerPixel)
{
  if (!pixel) return 0;
  switch (bytesPerPixel) {
  case 1: return *static_cast<const Uint8*>(pixel);
  case 2: return *static_cast<const Uint16*>(pixel);
  case 3: {
    auto pixelPtr = static_cast<const Uint8*>(pixel);
    Uint32 value = 0;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    for (int i = 0; i < 3; i++) { value |= pixelPtr[i] >> i * 8; }
#else
    for (int i = 0; i < 3; i++) { value |= pixelPtr[i] >> (2 - i) * 8; }
#endif // SDL_BYTEORDER == SDL_LIL_ENDIAN
    return value;
  }
  case 4: return *static_cast<const Uint32*>(pixel);
  default: return 0;
  }
}

constexpr Uint32
getPixelAt(const Surface& surface, int x, int y)
{
  return getPixel(pixelAt(surface, x, y),
                  surface.GetFormat().GetBytesPerPixel());
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
setPixelAt(Surface& surface, int x, int y, Uint32 value)
{
  setPixel(
    pixelAt(surface, x, y), value, surface.GetFormat().GetBytesPerPixel());
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_PIXEL_INCLUDED */
