#ifndef PIXEDIT_SRC_UTILS_PIX_WRITER_INCLUDED
#define PIXEDIT_SRC_UTILS_PIX_WRITER_INCLUDED

#include <SDL.h>
#include "PixFormat.hpp"

namespace pixedit {

inline size_t
writePixImage(SDL_RWops* rw, SDL_Surface* surface)
{
  PictureFormat format{surface->w, surface->h, surface->format->format};
  auto startPos = SDL_RWtell(rw);
  SDL_WriteLE32(rw, makeTag("RIFF"));

  auto dataSz = format.getSizeInBytes();
  Uint32 contentSz = 4 +    // PIX tag
                     16 +   // FMT tag and content
                     8 +    // DATA tag
                     dataSz // DATA content
    ;
  if (contentSz % 2) contentSz++;
  SDL_WriteLE32(rw, contentSz);
  SDL_WriteLE32(rw, makeTag("PIX "));

  SDL_WriteLE32(rw, makeTag("FMT "));
  SDL_WriteLE32(rw, 8);
  SDL_WriteLE16(rw, format.w);
  SDL_WriteLE16(rw, format.w);
  SDL_WriteLE32(rw, format.pixel);

  SDL_WriteLE32(rw, makeTag("DATA"));
  SDL_WriteLE32(rw, format.getSizeInBytes());

  auto rowSz = format.getRowSizeInBytes();
  const Uint8* src = static_cast<const Uint8*>(surface->pixels);
  for (int y = 0; y < format.h; ++y) {
    SDL_RWwrite(rw, src, 1, rowSz);
    src += surface->pitch;
  }
  if (dataSz % 2) SDL_WriteU8(rw, 0);
  auto sz = SDL_RWtell(rw) - startPos;
  if (sz == contentSz + 8) return sz;
  SDL_RWseek(rw, startPos, RW_SEEK_SET);
  return 0;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_PIX_WRITER_INCLUDED */
