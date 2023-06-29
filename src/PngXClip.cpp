#include "PngXClip.hpp"
#include <cstdlib>
#include <sstream>
#include <string>
#include <SDL_image.h>

namespace pixedit {

bool
copyToXClip(SDL_Surface* surface)
{
  if (surface == nullptr) { return false; }
  std::string filename = "temp.png";
  std::stringstream ss;
  ss << "xclip -selection clipboard -t image/png  -in " << filename;
  if (IMG_SavePNG(surface, filename.c_str()) != 0) return false;
  if (std::system(ss.str().c_str()) != 0) { return false; }
  ss.clear();
  ss << "rm " << filename;
  std::system(ss.str().c_str());
  return true;
}
} // namespace pixedit
