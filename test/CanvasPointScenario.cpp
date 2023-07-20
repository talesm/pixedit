#include "catch.hpp"
#include "Canvas.hpp"
#include "primitives/Point.hpp"

using namespace pixedit;

SCENARIO("Drawing a point", "[canvas]")
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
        WHEN("draw top left (0x0) pixel to color")
        {
          canvas | Point(0, 0);
          THEN("The pixel is changed to color")
          {
            REQUIRE(pixels[0] == color);
          }
        }
        WHEN("draw top right (7x0) pixel to color")
        {
          canvas | Point(7, 0);
          THEN("The pixel is changed to color")
          {
            REQUIRE(pixels[7] == color);
          }
        }
        WHEN("draw bottom right (7x7) pixel to color")
        {
          canvas | Point(7, 7);
          THEN("The pixel is changed to color")
          {
            REQUIRE(pixels[63] == color);
          }
        }
        AND_GIVEN("Pen is set to rectangle 2x3")
        {
          canvas | Pen(2, 3);
          WHEN("draw point at (1x1) to color")
          {
            canvas | Point(1, 1);
            THEN("The pixel is changed to color")
            {
              REQUIRE(pixels[0] != color);
              REQUIRE(pixels[1] == color);
              REQUIRE(pixels[2] == color);
              REQUIRE(pixels[3] != color);
              REQUIRE(pixels[8] != color);
              REQUIRE(pixels[9] == color);
              REQUIRE(pixels[10] == color);
              REQUIRE(pixels[11] != color);
              REQUIRE(pixels[16] != color);
              REQUIRE(pixels[17] == color);
              REQUIRE(pixels[18] == color);
              REQUIRE(pixels[19] != color);
              REQUIRE(pixels[25] != color);
              REQUIRE(pixels[26] != color);
            }
          }
        }
      }
      AND_GIVEN("Checkered pattern of black and white")
      {
        Uint32 colorA = 0xFFFF'FFFF, colorB = 0x0000'00FF;
        canvas | RawColorA(colorA) | RawColorB(colorB) | patterns::CHECKERED;
        WHEN("draw top left (0x0) pixel to colorA")
        {
          canvas | Point(0, 0);
          THEN("The pixel is changed to colorA")
          {
            REQUIRE(pixels[0] == colorA);
          }
        }
        WHEN("draw (1x0) pixel to colorB")
        {
          canvas | Point(1, 0);
          THEN("The pixel is changed to colorB")
          {
            REQUIRE(pixels[1] == colorB);
          }
        }
        WHEN("draw (0x1) pixel to colorB")
        {
          canvas | Point(0, 1);
          THEN("The pixel is changed to colorB")
          {
            REQUIRE(pixels[8] == colorB);
          }
        }
      }
    }
  }
}
