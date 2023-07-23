#include "loaders.hpp"
#include <fstream>
#include <unordered_map>
#include <vector>
#include <SDL_image.h>
#include "PictureBuffer.hpp"
#include "Surface.hpp"
#include "utils/PixReader.hpp"
#include "utils/replayPicture.hpp"

namespace pixedit {

static const std::vector<Id>&
getDefaultLoaderIds()
{
  static std::vector<Id> ids{loaders::PIX, loaders::SDL2_IMAGE, loaders::TEXT};
  return ids;
}

static Surface
doLoadSurface(const std::string& filename, Id loader)
{
  if (loader == loaders::PIX) {
    SDL_RWops* rw = SDL_RWFromFile(filename.c_str(), "rb");
    Surface s{readPixImage(rw), true};
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
#define makeLoaderWrapper(id)                                                  \
  static std::unique_ptr<PictureBuffer> loadBuffer_##id(                       \
    const std::string& filename)                                               \
  {                                                                            \
    auto s = doLoadSurface(filename, loaders::id);                             \
    if (!s) { return nullptr; }                                                \
    return std::make_unique<PictureBuffer>(                                    \
      PictureFile{filename, loadBuffer_##id, nullptr}, s, false);              \
  }

makeLoaderWrapper(PIX);
makeLoaderWrapper(SDL2_IMAGE);
makeLoaderWrapper(TEXT);

std::unique_ptr<PictureBuffer>
loadBuffer(const std::string& filename, Id loader)
{
  static std::unordered_map<IdRef, Loader> loaders{
    {loaders::PIX, loadBuffer_PIX},
    {loaders::SDL2_IMAGE, loadBuffer_SDL2_IMAGE},
    {loaders::TEXT, loadBuffer_TEXT},
  };
  if (loader.empty()) {
    for (Id id : getDefaultLoaderIds()) {
      auto s = doLoadSurface(filename, id);
      if (s) {
        return std::make_unique<PictureBuffer>(
          PictureFile{filename, loaders[id], nullptr}, std::move(s));
      }
    }
    return nullptr;
  }
  auto s = loadSurface(filename, loader);
  if (!s) { return nullptr; }
  return std::make_unique<PictureBuffer>(
    PictureFile{filename, loaders[loader], nullptr}, std::move(s));
}

Surface
loadSurface(const std::string& filename, Id loader)
{
  if (loader.empty()) {
    for (Id id : getDefaultLoaderIds()) {
      auto s = doLoadSurface(filename, id);
      if (s) { return s; }
    }
    return nullptr;
  }
  return doLoadSurface(filename, loader);
}

} // namespace pixedit
