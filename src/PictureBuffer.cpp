#include "PictureBuffer.hpp"
#include <SDL_image.h>

namespace pixedit {

PictureBuffer::PictureBuffer(std::string filename)
  : PictureBuffer(std::move(filename), IMG_Load(filename.c_str()), true)
{
  if (surface) { makeSnapshot(); }
}

bool
PictureBuffer::save()
{
  return IMG_SavePNG(surface, filename.c_str()) == 0;
}

void
PictureBuffer::makeSnapshot()
{
  if (!surface) return;
  if (historyPoint != history.end()) {
    history.erase(++historyPoint, history.end());
  }
  historyPoint = history.emplace(history.end(), surface);
}

bool
PictureBuffer::undo()
{
  if (!surface || history.empty() || historyPoint == history.begin()) {
    return false;
  }
  SDL_FreeSurface(surface);
  --historyPoint;
  surface = historyPoint->recover();
  return true;
}

bool
PictureBuffer::redo()
{
  if (!surface || history.empty()) { return false; }
  ++historyPoint;
  if (historyPoint == history.end()) {
    --historyPoint;
    return false;
  }
  SDL_FreeSurface(surface);
  surface = historyPoint->recover();
  return false;
}

} // namespace pixedit
