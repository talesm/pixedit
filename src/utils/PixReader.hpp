#ifndef PIXEDIT_SRC_UTILS_PIX_READER_INCLUDED
#define PIXEDIT_SRC_UTILS_PIX_READER_INCLUDED

#include <concepts>
#include <span>
#include <vector>
#include <SDL.h>
#include "PixFormat.hpp"

namespace pixedit {

template<std::invocable<PictureFormat, std::span<Uint8>> CALLBACK>
bool
readPixImage(SDL_RWops* rw, CALLBACK callback)
{
  if (!rw) { return false; }
  if (SDL_ReadLE32(rw) != makeTag("RIFF")) { return false; }
  Uint32 size = SDL_ReadLE32(rw);
  if (size < 4 || SDL_ReadLE32(rw) != makeTag("PIX")) { return false; }
  size -= 4;
  if (size < 4 || SDL_ReadLE32(rw) != makeTag("FMT")) { return false; }
  size -= 4;
  Uint32 fmtSize;
  if (size < 12 || (fmtSize = SDL_ReadLE32(rw)) < 8) { return false; }
  size -= 4;
  if (fmtSize % 2) fmtSize++;
  PictureFormat format{1, 1, SDL_PIXELFORMAT_RGBA32};
  format.w = SDL_ReadLE16(rw);
  fmtSize -= 2;
  format.h = SDL_ReadLE16(rw);
  fmtSize -= 2;
  format.pixel = SDL_ReadLE32(rw);
  fmtSize -= 4;
  SDL_RWseek(rw, fmtSize, RW_SEEK_CUR);
  size -= fmtSize;

  if (size < 8 || SDL_ReadLE32(rw) != makeTag("DATA")) { return false; }
  Uint32 dataSize = SDL_ReadLE32(rw);
  size -= 8;
  Uint32 expectedSize = format.getSizeInBytes();
  if (size < expectedSize || dataSize < expectedSize) { return false; }
  std::vector<Uint8> pixels(expectedSize);
  SDL_RWread(rw, pixels.data(), 1, pixels.size());

  callback(format, std::span{pixels});
  return true;
}

SDL_Surface*
readPixImage(SDL_RWops* rw);

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_PIX_READER_INCLUDED */
