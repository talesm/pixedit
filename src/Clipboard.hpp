#ifndef PIXEDIT_SRC_CLIPBOARD_INCLUDED
#define PIXEDIT_SRC_CLIPBOARD_INCLUDED

#include <SDL.h>

namespace pixedit {

/// @brief Clipboard for images
struct Clipboard
{
  SDL_Surface* get();

  bool set(SDL_Surface* surface);
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_CLIPBOARD_INCLUDED */
