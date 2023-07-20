#include "loaders.hpp"
#include <fstream>
#include <vector>
#include <SDL_image.h>
#include "PictureBuffer.hpp"
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
doLoadSurface(const std::string& filename, Id loader)
{
  if (loader == loaders::PIX) { return nullptr; }
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
