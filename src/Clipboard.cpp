#include "Clipboard.hpp"
#include "clipboards/FallbackClip.hpp"
#include "clipboards/PngXClip.hpp"

namespace pixedit {

namespace defaults {
extern const int CLIPBOARD_MANAGER;

namespace clipboards {
extern const int FALLBACK;
extern const int XCLIP;
} // namespace clipboards

} // namespace defaults
using namespace defaults;

SDL_Surface*
Clipboard::get()
{
  if (CLIPBOARD_MANAGER == clipboards::XCLIP) return copyFromXClip();
  return copyFromFallback();
}

bool
Clipboard::set(SDL_Surface* surface)
{
  if (CLIPBOARD_MANAGER == clipboards::XCLIP) return copyToXClip(surface);
  return copyToFallback(surface);
}

} // namespace pixedit
