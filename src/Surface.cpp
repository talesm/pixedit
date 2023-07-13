#include "Surface.hpp"
#include <fstream>
#include <SDL_image.h>
#include "utils/dumpSurface.hpp"
#include "utils/replayPicture.hpp"

namespace pixedit {

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
    dump::surface(out, *this, 80, true);
    return out.good();
  }
  return false;
}

Surface
Surface::load(const std::string& filename)
{
  if (filename.ends_with(".txt")) {
    std::ifstream fStream(filename);
    return replayPicture(fStream);
  }
  return {IMG_Load(filename.c_str()), true};
}

} // namespace pixedit
