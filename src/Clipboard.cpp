#include "Clipboard.hpp"
#include "clipboards/PngXClip.hpp"

namespace pixedit {

SDL_Surface*
Clipboard::get()
{
  return copyFromXClip();
}

bool
Clipboard::set(SDL_Surface* surface)
{
  return copyToXClip(surface);
}

} // namespace pixedit
