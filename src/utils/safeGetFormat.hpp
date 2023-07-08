#ifndef PIXEDIT_SRC_UTILS_SAFE_GET_FORMAT_INCLUDED
#define PIXEDIT_SRC_UTILS_SAFE_GET_FORMAT_INCLUDED

#include <SDL.h>

namespace pixedit {

/// @brief Safely gets format even if surface is null
/// @param surface
/// @return
constexpr SDL_PixelFormat*
safeGetFormat(SDL_Surface* surface)
{
  if (!surface) return nullptr;
  return surface->format;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_SAFE_GET_FORMAT_INCLUDED */
