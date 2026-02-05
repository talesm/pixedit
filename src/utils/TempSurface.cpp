#include "TempSurface.hpp"
#include <ctime>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include "paths.hpp"

namespace pixedit {

std::string
makeTempFilename(std::string_view prefix, std::string_view suffix)
{
  unsigned randomContent = time(nullptr) ^ clock();
  std::stringstream ss;
  ss << getPrefPath() << prefix << std::hex << randomContent << suffix;
  return ss.str();
}

TempSurface::TempSurface()
  : filename(makeTempFilename("temp_", ".png"))
{
}

TempSurface::TempSurface(const Surface& surface)
  : TempSurface()
{ surface.SavePNG(filename); }

TempSurface::TempSurface(const Surface& surface, std::string filename)
  : filename(std::move(filename))
{ surface.SavePNG(filename); }

Surface
TempSurface::recover() const
{
  Surface surface = SDL::LoadSurface(filename);
  if (!surface) { throw std::runtime_error{"Can not recover"}; }
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
