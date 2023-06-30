#include "catch.hpp"
#include "Canvas.hpp"

using namespace pixedit;

SCENARIO("Drawing a point", "[canvas]")
{
  GIVEN("a Canvas")
  {
    Canvas canvas;
    AND_GIVEN("it is associated with a 8x8 surface")
    {
      Uint32 pixels[8 * 8] = {0}; // Blank
      SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels,
                                                      8,
                                                      8,
                                                      32,
                                                      8 * sizeof(Uint32),
                                                      0xFF00'0000,
                                                      0x00FF'0000,
                                                      0x0000'FF00,
                                                      0x0000'00FF);
      canvas.setSurface(surface);
      AND_GIVEN("color is set to white")
      {
        Uint32 color = 0xFFFFFFFF;
        canvas | setColor(color);
        WHEN("draw top left (0x0) pixel to color")
        {
          canvas | drawPoint(0, 0);
          THEN("The pixel is changed to color")
          {
            REQUIRE(pixels[0] == color);
          }
        }
        WHEN("draw top right (7x0) pixel to color")
        {
          canvas | drawPoint(7, 0);
          THEN("The pixel is changed to color")
          {
            REQUIRE(pixels[7] == color);
          }
        }
        WHEN("draw bottom right (7x7) pixel to color")
        {
          canvas | drawPoint(7, 7);
          THEN("The pixel is changed to color")
          {
            REQUIRE(pixels[63] == color);
          }
        }
      }
    }
  }
}
