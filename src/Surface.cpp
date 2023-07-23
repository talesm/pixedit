#include "Surface.hpp"
#include "loaders.hpp"
#include "savers.hpp"

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
  return saveSurface(*this, filename);
}

Surface
Surface::load(const std::string& filename)
{
  return loadSurface(filename);
}

} // namespace pixedit
