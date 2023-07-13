#include "catch.hpp"
#include "Canvas.hpp"
#include "primitives/Rect.hpp"

using namespace pixedit;

SCENARIO("Drawing a filled rectangle", "[canvas]")
{
  GIVEN("a Canvas")
  {
    Canvas canvas;
    AND_GIVEN("it is associated with a 8x8 surface")
    {
      Uint32 pixels[8 * 8] = {0}; // Blank
      auto surface = Surface{SDL_CreateRGBSurfaceFrom(pixels,
                                                      8,
                                                      8,
                                                      32,
                                                      8 * sizeof(Uint32),
                                                      0xFF00'0000,
                                                      0x00FF'0000,
                                                      0x0000'FF00,
                                                      0x0000'00FF),
                             true};
      canvas.setSurface(surface);
      AND_GIVEN("color is set to white")
      {
        Uint32 color = 0xFFFFFFFF;
        canvas | RawColorA(color);
        WHEN("fill rect from top left (0x0) to (2x0) to color")
        {
          canvas | FillRectTo(0, 0, 2, 1);
          THEN("The pixel is changed to color")
          {
            REQUIRE(pixels[0] == color);
            REQUIRE(pixels[1] == color);
            REQUIRE(pixels[2] == color);
            REQUIRE(pixels[3] != color);
            REQUIRE(pixels[8] == color);
            REQUIRE(pixels[10] == color);
            REQUIRE(pixels[16] != color);
          }
        }
        WHEN("fill rect backwards")
        {
          canvas | FillRectTo(2, 3, 0, 1);
          THEN("The pixel is changed to color")
          {
            REQUIRE(pixels[8] == color);
          }
        }
      }
      AND_GIVEN("Checkered pattern of black and white")
      {
        Uint32 colorA = 0xFFFF'FFFF, colorB = 0x0000'00FF;
        canvas | RawColorA(colorA) | RawColorB(colorB) | patterns::CHECKERED;
        WHEN("fill rect from top left (0x0) to (2x0) to color")
        {
          canvas | FillRectTo(0, 0, 2, 1);
          THEN("The pixel is changed to color")
          {
            REQUIRE(pixels[0] == colorA);
            REQUIRE(pixels[1] == colorB);
            REQUIRE(pixels[2] == colorA);
            REQUIRE(pixels[3] != colorA);
            REQUIRE(pixels[3] != colorB);
            REQUIRE(pixels[8] == colorB);
            REQUIRE(pixels[10] == colorB);
            REQUIRE(pixels[16] != colorA);
            REQUIRE(pixels[16] != colorB);
          }
        }
      }
    }
  }
}
