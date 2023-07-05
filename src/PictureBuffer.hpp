#ifndef PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED
#define PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED

#include <list>
#include <string>
#include <SDL.h>
#include "TempSurface.hpp"

namespace pixedit {
/**
 * The editing target
 */
class PictureBuffer
{
public: // TODO Make this private too
  std::string filename;
  SDL_Surface* surface = nullptr;

private:
  std::list<TempSurface> history;
  std::list<TempSurface>::iterator historyPoint = history.end();

public:
  PictureBuffer() = default;
  PictureBuffer(std::string filename);
  PictureBuffer(std::string filename, SDL_Surface* surface, bool owner = false)
    : filename(std::move(filename))
    , surface(surface)
  {
    if (!owner) surface->refcount++;
  }

  ~PictureBuffer() { SDL_FreeSurface(surface); }
  PictureBuffer(const PictureBuffer&) = delete;
  PictureBuffer(PictureBuffer&& rhs)
    : filename(std::move(rhs.filename))
    , surface(rhs.surface)
  {
    rhs.surface = nullptr;
  }
  PictureBuffer& operator=(PictureBuffer rhs)
  {
    std::swap(filename, rhs.filename);
    std::swap(surface, rhs.surface);
    return *this;
  }

  bool save();

  void makeSnapshot();

  bool undo();

  bool redo();
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED */
