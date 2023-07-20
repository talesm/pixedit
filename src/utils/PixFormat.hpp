#ifndef PIXEDIT_SRC_UTILS_PIX_FORMAT_INCLUDED
#define PIXEDIT_SRC_UTILS_PIX_FORMAT_INCLUDED

#include <SDL.h>

namespace pixedit {

struct PictureFormat
{
  int w, h;
  Uint32 pixel;

  constexpr Uint32 getRowSizeInBytes() const
  {
    Uint32 sz = w * SDL_BITSPERPIXEL(pixel);
    return sz / 8 + (sz % 8 ? 1 : 0);
  }

  constexpr Uint32 getSizeInBytes() const { return getRowSizeInBytes() * h; }
};

constexpr Uint32
makeTag(const char* name)
{
  if (!name) return ' ' | ' ' << 8 | ' ' << 16 | ' ' << 24;
  Uint32 result = 0;
  for (int i = 0; i < 4; ++i) {
    if (name[i] == 0) {
      for (; i < 4; ++i) { result |= ' ' << (8 * i); }
      break;
    }
    result |= name[i] << (8 * i);
  }
  return result;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_PIX_FORMAT_INCLUDED */
