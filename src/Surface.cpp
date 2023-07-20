#include "Surface.hpp"
#include <fstream>
#include <SDL_image.h>
#include "loaders.hpp"
#include "utils/dumpSurface.hpp"

namespace pixedit {

Surface
Surface::clone() const
{
  if (!surface) { return nullptr; }
  return cloneWith(surface->format);
}

Surface
Surface::cloneWith(SDL_PixelFormatEnum format) const
{
  if (!surface) { return nullptr; }
  return {SDL_ConvertSurfaceFormat(surface, format, 0), true};
}

Surface
Surface::cloneWith(const SDL_PixelFormat* format) const
{
  if (!surface) { return nullptr; }
  return {SDL_ConvertSurface(surface, format, 0), true};
}

bool
Surface::save(const std::string& filename) const
{
  // TODO a Loader library component
  if (filename.ends_with(".png")) {
    return IMG_SavePNG(surface, filename.c_str()) == 0;
  }
  if (filename.ends_with(".jpg") || filename.ends_with(".jpeg")) {
    return IMG_SaveJPG(surface, filename.c_str(), 90) == 0;
  }
  if (filename.ends_with(".bmp")) {
    return SDL_SaveBMP(surface, filename.c_str()) == 0;
  }
  if (filename.ends_with(".txt")) {
    std::ofstream out(filename);
    dump::surface(out, cloneWith(DEFAULT_FORMAT), 80, true);
    return out.good();
  }
  return false;
}

Surface
Surface::load(const std::string& filename)
{
  return loadSurface(filename);
}

} // namespace pixedit
