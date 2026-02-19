#ifndef PIXEDIT_SRC_SURFACE_INCLUDED
#define PIXEDIT_SRC_SURFACE_INCLUDED

#include <SDL3pp/SDL3pp.h>

namespace pixedit {

using SDL::Surface;

constexpr auto DEFAULT_FORMAT = SDL::PIXELFORMAT_ABGR32;

inline Surface
createMask(int w, int h)
{
  Surface surface({w, h}, SDL::PIXELFORMAT_INDEX8);
  auto palette = surface.CreatePalette();
  static SDL_Color colors[2] = {{0, 0, 0, 0}, {255, 255, 255, 0}};
  palette.SetColors(colors, 0);
  return surface;
}
} // namespace pixedit

#endif /* PIXEDIT_SRC_SURFACE_INCLUDED */
