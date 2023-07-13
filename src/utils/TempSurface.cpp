#include "TempSurface.hpp"
#include <ctime>
#include <filesystem>
#include <sstream>
#include <stdexcept>

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

TempSurface::TempSurface(Surface surface)
  : TempSurface()
{
  if (!surface.save(filename)) { throw std::runtime_error{"Can not save"}; }
}

TempSurface::TempSurface(Surface surface, std::string filename)
  : filename(std::move(filename))
{
  if (!surface.save(filename)) { throw std::runtime_error{"Can not save"}; }
}

Surface
TempSurface::recover() const
{
  Surface surface = Surface::load(filename);
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
