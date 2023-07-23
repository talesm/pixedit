#include "catch.hpp"
#include "utils/PixWriter.hpp"

using namespace pixedit;

TEST_CASE("SimplestFile write", "[PixWriter]")
{
  Uint8 fileData[1024];
  auto rw = SDL_RWFromMem(fileData, sizeof(fileData));

  SDL_Surface* surface =
    SDL_CreateRGBSurfaceWithFormat(0, 1, 1, 8, SDL_PIXELFORMAT_RGB332);

  auto sz = writePixImage(rw, surface);
  CHECK(sz == 38);

  SDL_FreeSurface(surface);
  SDL_RWclose(rw);
}
