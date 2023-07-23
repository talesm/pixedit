#include "savers.hpp"
#include <fstream>
#include <SDL_image.h>
#include "PictureBuffer.hpp"
#include "Surface.hpp"
#include "utils/PixWriter.hpp"
#include "utils/dumpSurface.hpp"

namespace pixedit {

bool
saveBuffer(const PictureBuffer& buffer, const std::string& filename, Id saver)
{
  return saveSurface(buffer.getSurface(), filename, saver);
}

static bool
doSaveSurface(const Surface& surface, const std::string& filename, Id saver)
{
  if (saver == savers::PIX) {
    SDL_RWops* rw = SDL_RWFromFile(filename.c_str(), "wb");
    if (!rw) return false;
    auto sz = writePixImage(rw, surface.get());
    SDL_RWclose(rw);
    return sz > 0;
  }
  if (saver == savers::SDL2_IMAGE_PNG)
    return IMG_SavePNG(surface.get(), filename.c_str()) == 0;
  if (saver == savers::SDL2_IMAGE_JPEG)
    return IMG_SaveJPG(surface.get(), filename.c_str(), 90) == 0;
  if (saver == savers::SDL2_BMP)
    return SDL_SaveBMP(surface.get(), filename.c_str()) == 0;
  if (saver == savers::TEXT) {
    std::ofstream out(filename);
    dump::surface(out, surface.cloneWith(Surface::DEFAULT_FORMAT), 80, true);
    return out.good();
  }
  return false;
}

Saver
savers::get(Id saver)
{
#define makeSaver(ID)                                                          \
  [](const PictureBuffer* buffer, const std::string& filename) {               \
    return doSaveSurface(buffer->getSurface(), filename, ID);                  \
  };

  if (saver == savers::PIX) return makeSaver(savers::PIX);
  if (saver == savers::SDL2_IMAGE_PNG) return makeSaver(savers::SDL2_IMAGE_PNG);
  if (saver == savers::SDL2_IMAGE_JPEG)
    return makeSaver(savers::SDL2_IMAGE_JPEG);
  if (saver == savers::SDL2_BMP) return makeSaver(savers::SDL2_BMP);
  if (saver == savers::TEXT) return makeSaver(savers::TEXT);
  return nullptr;

#undef makeSaver
}

Id
saverForFile(const std::string& filename)
{
  if (filename.ends_with(".pix")) return savers::PIX;
  if (filename.ends_with(".png")) return savers::SDL2_IMAGE_PNG;
  if (filename.ends_with(".jpg") || filename.ends_with(".jpeg"))
    return savers::SDL2_IMAGE_JPEG;
  if (filename.ends_with(".bmp")) return savers::SDL2_BMP;
  if (filename.ends_with(".txt")) return savers::TEXT;
  return {};
}

bool
saveSurface(const Surface& surface, const std::string& filename, Id saver)
{
  if (saver.empty()) {
    return doSaveSurface(surface, filename, saverForFile(filename));
  }
  return doSaveSurface(surface, filename, saver);
}

} // namespace pixedit
