#include "Clipboard.hpp"
#include "PngXClip.hpp"

namespace pixedit {

SDL_Surface*
Clipboard::get()
{
  return nullptr;
}

bool
Clipboard::set(SDL_Surface* surface)
{
  return copyToXClip(surface);
}

} // namespace pixedit
