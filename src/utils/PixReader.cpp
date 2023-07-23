#include "PixReader.hpp"

namespace pixedit {

static SDL_Surface*
makePixSurface(const PictureFormat& format, std::span<Uint8> data)
{
  if (format.pixel == SDL_PIXELFORMAT_UNKNOWN) { return nullptr; }
  SDL_Surface* s = SDL_CreateRGBSurfaceWithFormat(
    0, format.w, format.h, SDL_BITSPERPIXEL(format.pixel), format.pixel);
  if (!s) { return nullptr; }
  if (SDL_ISPIXELFORMAT_INDEXED(format.pixel)) {
    SDL_Color grayscale[256];
    for (int i = 0; i < 256; ++i) {
      grayscale[i] = {Uint8(i), Uint8(i), Uint8(i), 255};
    }

    SDL_SetPaletteColors(s->format->palette, grayscale, 0, 256);
  }
  Uint8* pixels = static_cast<Uint8*>(s->pixels);
  auto it = data.begin();
  for (int y = 0; y < format.h; ++y) {
    auto end = it + format.getRowSizeInBytes();
    std::copy(it, end, pixels);
    it = end;
    pixels += s->pitch;
  }
  return s;
}

SDL_Surface*
readPixImage(SDL_RWops* rw)
{
  SDL_Surface* surface = nullptr;
  readPixImage(rw, [&surface](const PictureFormat& format, auto data) {
    surface = makePixSurface(format, data);
  });
  return surface;
}

} // namespace pixedit
