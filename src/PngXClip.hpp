#ifndef PIXEDIT_SRC_PNG_XCLIP_HPP_INCLUDED
#define PIXEDIT_SRC_PNG_XCLIP_HPP_INCLUDED

#include <SDL.h>

namespace pixedit {

/**
 * Copy surface to clipboard
 *
 * @param surface the surface to be copied
 *
 * @return true if copied successfully
 */
bool
copyToXClip(SDL_Surface* surface);
} // namespace pixedit

#endif /* PIXEDIT_SRC_PNG_XCLIP_HPP_INCLUDED */
