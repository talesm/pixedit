#include "loaders.hpp"
#include <fstream>
#include <vector>
#include <SDL_image.h>
#include "PictureBuffer.hpp"
#include "PixReader.hpp"
#include "Surface.hpp"
#include "utils/replayPicture.hpp"

namespace pixedit {

static const std::vector<Id>&
getDefaultLoaders()
{
  static std::vector<Id> ids{loaders::PIX, loaders::SDL2_IMAGE, loaders::TEXT};
  return ids;
}

std::unique_ptr<PictureBuffer>
loadBuffer(const std::string& filename, Id loader)
{
  auto s = loadSurface(filename, loader);
  if (!s) { return nullptr; }
  return std::make_unique<PictureBuffer>(filename, std::move(s));
}

static Surface
makePixSurface(const PictureFormat& format, std::span<Uint8> data)
{
  SDL_PixelFormatEnum pixelFormat;
  switch (format.bitsPerPixel) {
  case 32: pixelFormat = SDL_PIXELFORMAT_ABGR32; break;
  case 24: pixelFormat = SDL_PIXELFORMAT_BGR24; break;
  case 16: pixelFormat = SDL_PIXELFORMAT_BGR565; break;
  case 8: pixelFormat = SDL_PIXELFORMAT_INDEX8; break;
  default: return nullptr;
  }
  SDL_Surface* s = SDL_CreateRGBSurfaceWithFormat(
    0, format.w, format.h, format.bitsPerPixel, pixelFormat);
  if (!s) { return nullptr; }
  if (SDL_ISPIXELFORMAT_INDEXED(pixelFormat)) {
    SDL_Color grayscale[256];
    for (int i = 0; i < 256; ++i) {
      grayscale[i] = {Uint8(i), Uint8(i), Uint8(i), 255};
    }

    SDL_SetPaletteColors(s->format->palette, grayscale, 0, 256);
  }
  auto bytesPerPixel = s->format->BytesPerPixel;
  Uint8* pixels = static_cast<Uint8*>(s->pixels);
  auto it = data.begin();
  for (int y = 0; y < format.h; ++y) {
    auto end = it + bytesPerPixel * format.w;
    std::copy(it, end, pixels);
    it = end;
    pixels += s->pitch;
  }
  return {s, true};
}

static Surface
doLoadSurface(const std::string& filename, Id loader)
{
  if (loader == loaders::PIX) {
    Surface s;
    SDL_RWops* rw = SDL_RWFromFile(filename.c_str(), "rb");
    readPixImage(rw, [&s](const PictureFormat& format, std::span<Uint8> data) {
      s = makePixSurface(format, data);
    });
    SDL_RWclose(rw);
    return s;
  }
  if (loader == loaders::SDL2_IMAGE) {
    return {IMG_Load(filename.c_str()), true};
  }
  if (loader == loaders::TEXT) {
    std::ifstream fStream(filename);
    return replayPicture(fStream);
  }
  return nullptr;
}

Surface
loadSurface(const std::string& filename, Id loader)
{
  if (loader.empty()) {
    for (Id id : getDefaultLoaders()) {
      auto s = doLoadSurface(filename, id);
      if (s) { return s; }
    }
    return nullptr;
  }
  return doLoadSurface(filename, loader);
}

} // namespace pixedit
