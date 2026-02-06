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
  : filename_(makeTempFilename("temp_", ".png"))
{
}

TempSurface::TempSurface(const Surface& surface)
  : TempSurface()
{ surface.SavePNG(filename_); }

TempSurface::TempSurface(const Surface& surface, std::string filename)
  : filename_(std::move(filename))
{ surface.SavePNG(filename); }

Surface
TempSurface::recover() const
{
  Surface surface = SDL::LoadSurface(filename_);
  if (!surface) { throw std::runtime_error{"Can not recover"}; }
  return surface;
}

void
TempSurface::reset()
{
  if (filename_.empty()) return;
  namespace fs = std::filesystem;
  fs::remove(filename_);
  filename_.clear();
}

} // namespace pixedit
