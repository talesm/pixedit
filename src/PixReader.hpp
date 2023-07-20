#ifndef PIXEDIT_SRC_PIX_READER_INCLUDED
#define PIXEDIT_SRC_PIX_READER_INCLUDED

#include <concepts>
#include <span>
#include <vector>
#include <SDL.h>

namespace pixedit {

struct PictureFormat
{
  int w, h, bitsPerPixel, flags;
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
  if (size < 10 || (fmtSize = SDL_ReadLE32(rw)) < 6) { return false; }
  size -= 4;
  if (fmtSize % 2) fmtSize++;
  PictureFormat format{1, 1, 8, 1};
  format.w = SDL_ReadLE16(rw);
  fmtSize -= 2;
  format.h = SDL_ReadLE16(rw);
  fmtSize -= 2;
  format.bitsPerPixel = SDL_ReadU8(rw);
  fmtSize -= 1;
  format.flags = SDL_ReadU8(rw);
  fmtSize -= 1;
  if (format.flags != 0 && (format.flags & 0x1) == 0) { return false; }
  SDL_RWseek(rw, fmtSize, RW_SEEK_CUR);
  size -= fmtSize;

  if (size < 8 || SDL_ReadLE32(rw) != makeTag("DATA")) { return false; }
  Uint32 dataSize = SDL_ReadLE32(rw);
  size -= 8;
  Uint32 expectedSize = format.w * format.h * format.bitsPerPixel;
  expectedSize = expectedSize / 8 + (expectedSize % 8 ? 1 : 0);
  if (size < expectedSize || dataSize < expectedSize) { return false; }
  std::vector<Uint8> pixels(expectedSize);
  SDL_RWread(rw, pixels.data(), 1, pixels.size());

  callback(format, std::span{pixels});
  return true;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_PIX_READER_INCLUDED */
