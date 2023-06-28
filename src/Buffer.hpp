#ifndef PIXEDIT_SRC_BUFFER_HPP_INCLUDED
#define PIXEDIT_SRC_BUFFER_HPP_INCLUDED

#include <string>
#include <SDL.h>

namespace pixedit {
/**
 * The editing target
 */
struct Buffer
{
  std::string filename;
  SDL_Surface* surface = nullptr;

  Buffer() = default;
  Buffer(std::string filename);
  Buffer(std::string filename, SDL_Surface* surface, bool owner = false)
    : filename(std::move(filename))
    , surface(surface)
  {
    if (!owner) surface->refcount++;
  }

  ~Buffer() { SDL_FreeSurface(surface); }
  Buffer(const Buffer&) = delete;
  Buffer(Buffer&& rhs)
    : filename(std::move(rhs.filename))
    , surface(rhs.surface)
  {
    rhs.surface = nullptr;
  }
  Buffer& operator=(Buffer rhs)
  {
    std::swap(filename, rhs.filename);
    std::swap(surface, rhs.surface);
    return *this;
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_BUFFER_HPP_INCLUDED */
