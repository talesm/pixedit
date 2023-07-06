#include "TempSurface.hpp"
#include <ctime>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <SDL_image.h>

namespace pixedit {

std::string
makeTempFilename(std::string_view prefix, std::string_view suffix)
{
  unsigned suffixNumber = time(nullptr) ^ clock();
  std::stringstream ss;
  ss << prefix << std::hex << suffixNumber << suffix;
  return ss.str();
}

TempSurface::TempSurface()
  : filename(makeTempFilename("temp_", ".png"))
{
}

TempSurface::TempSurface(SDL_Surface* surface)
  : TempSurface()
{
  if (IMG_SavePNG(surface, filename.c_str()) < 0) {
    throw std::runtime_error{"Can not save"};
  }
}

SDL_Surface*
TempSurface::recover() const
{
  SDL_Surface* surface = IMG_Load(filename.c_str());
  if (surface == nullptr) { throw std::runtime_error{"Can not recover"}; }
  return surface;
}

void
TempSurface::reset()
{
  if (filename.empty()) return;
  namespace fs = std::filesystem;
  fs::remove(filename);
  filename.clear();
}

} // namespace pixedit
