#include "FallbackClip.hpp"
#include <SDL_image.h>

namespace pixedit {

static const char* tempClip = "fallback_clipboard.png";

bool
copyToFallback(SDL_Surface* surface)
{
  if (surface == nullptr) { return false; }
  return IMG_SavePNG(surface, tempClip) == 0;
}

SDL_Surface*
copyFromFallback()
{
  return IMG_Load(tempClip);
}

} // namespace pixedit
