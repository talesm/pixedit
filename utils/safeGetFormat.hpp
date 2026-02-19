#ifndef PIXEDIT_SRC_UTILS_SAFE_GET_FORMAT_INCLUDED
#define PIXEDIT_SRC_UTILS_SAFE_GET_FORMAT_INCLUDED

#include <SDL3/SDL.h>

namespace pixedit {

/// @brief Safely gets format even if surface is null
/// @param surface
/// @return
inline const SDL_PixelFormatDetails*
safeGetFormat(SDL_Surface* surface)
{
  if (!surface) return nullptr;
  return SDL_GetPixelFormatDetails(surface->format);
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_SAFE_GET_FORMAT_INCLUDED */
