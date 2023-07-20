#include "PixReader.hpp"
#include "catch.hpp"

using namespace pixedit;

TEST_CASE("SimplestFile", "[PixReader]")
{
  Uint8 fileData[] = {'R', 'I', 'F', 'F', 28,  0,   0, 0, 'P', 'I', 'X', ' ',
                      'F', 'M', 'T', ' ', 6,   0,   0, 0, 1,   0,   1,   0,
                      8,   1,   'D', 'A', 'T', 'A', 1, 0, 0,   0,   1,   0};
  PictureFormat format;
  std::vector<Uint8> pixels;
  auto callback = [&](PictureFormat f, std::span<Uint8> ps) {
    format = f;
    pixels = std::vector<Uint8>{ps.begin(), ps.end()};
  };

  auto rw = SDL_RWFromConstMem(fileData, sizeof(fileData));
  REQUIRE(readPixImage(rw, callback) == true);
  SDL_RWclose(rw);
  CHECK(format.bitsPerPixel == 8);
  CHECK(format.w == 1);
  CHECK(format.h == 1);
  CHECK(format.flags == 1);
  CHECK(pixels.size() == 1);
  REQUIRE(pixels.at(0) == 1);
}
TEST_CASE("SimplestFile32", "[PixReader]")
{
  Uint8 fileData[] = {'R', 'I', 'F', 'F', 30, 0, 0, 0, 'P', 'I', 'X', ' ', 'F',
                      'M', 'T', ' ', 6,   0,  0, 0, 1, 0,   1,   0,   32,  1,
                      'D', 'A', 'T', 'A', 4,  0, 0, 0, 255, 127, 63,  255};
  PictureFormat format;
  std::vector<Uint8> pixels;
  auto callback = [&](PictureFormat f, std::span<Uint8> ps) {
    format = f;
    pixels = std::vector<Uint8>{ps.begin(), ps.end()};
  };

  auto rw = SDL_RWFromConstMem(fileData, sizeof(fileData));
  REQUIRE(readPixImage(rw, callback) == true);
  SDL_RWclose(rw);
  CHECK(format.bitsPerPixel == 32);
  CHECK(format.w == 1);
  CHECK(format.h == 1);
  CHECK(format.flags == 1);
  CHECK(pixels.size() == 4);
  REQUIRE(pixels.at(0) == 255);
  REQUIRE(pixels.at(1) == 127);
  REQUIRE(pixels.at(2) == 63);
  REQUIRE(pixels.at(3) == 255);
}

TEST_CASE("Invalid image file", "[PixReader]")
{
  char fileData[] = {'R', 'I', 'F', 'F', 28,  0,   0, 0, 'P', 'I', 'X', ' ',
                     'F', 'M', 'T', ' ', 6,   0,   0, 0, 1,   0,   1,   0,
                     8,   1,   'D', 'A', 'T', 'A', 1, 0, 0,   0,   0,   0};
  SDL_RWops* rw = nullptr;
  SECTION("Empty")
  {
    rw = SDL_RWFromConstMem(fileData, 0);
    REQUIRE_FALSE(readPixImage(rw, [&](auto f, std::span<Uint8> ps) {}));
  }
  SECTION("Non RIFF")
  {
    fileData[1] = 'O';
    rw = SDL_RWFromConstMem(fileData, sizeof(fileData));
    REQUIRE_FALSE(readPixImage(rw, [&](auto f, auto ps) {}));
  }
  SECTION("Non PIX")
  {
    fileData[9] = 'O';
    rw = SDL_RWFromConstMem(fileData, sizeof(fileData));
    REQUIRE_FALSE(readPixImage(rw, [&](auto f, auto ps) {}));
  }
  SECTION("Non FMT")
  {
    fileData[13] = 'O';
    rw = SDL_RWFromConstMem(fileData, sizeof(fileData));
    REQUIRE_FALSE(readPixImage(rw, [&](auto f, auto ps) {}));
  }
  SECTION("Non FMT")
  {
    fileData[13] = 'O';
    rw = SDL_RWFromConstMem(fileData, sizeof(fileData));
    REQUIRE_FALSE(readPixImage(rw, [&](auto f, auto ps) {}));
  }
  SECTION("Wrong variant")
  {
    fileData[25] = 2;
    rw = SDL_RWFromConstMem(fileData, sizeof(fileData));
    REQUIRE_FALSE(readPixImage(rw, [&](auto f, auto ps) {}));
  }
  SECTION("No data")
  {
    fileData[4] = 18;
    rw = SDL_RWFromConstMem(fileData, sizeof(fileData));
    REQUIRE_FALSE(readPixImage(rw, [&](auto f, auto ps) {}));
  }
  SECTION("Invalid data size")
  {
    fileData[30] = 0;
    rw = SDL_RWFromConstMem(fileData, sizeof(fileData));
    REQUIRE_FALSE(readPixImage(rw, [&](auto f, auto ps) {}));
    fileData[30] = 2;
    REQUIRE_FALSE(readPixImage(rw, [&](auto f, auto ps) {}));
  }

  if (rw) { SDL_RWclose(rw); }
}
