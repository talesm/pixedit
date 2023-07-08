#ifndef PIXEDIT_SRC_UTILS_COLOR_INCLUDED
#define PIXEDIT_SRC_UTILS_COLOR_INCLUDED

#include <SDL.h>

namespace pixedit {

using Color = SDL_Color;

// Forward decl
struct ColorB;

/// @brief Sets color
using RawColorA = RawColor;

/// @brief Sets secondary color
struct RawColorB
{
  RawColor color;
};

/// @brief Color conversion utilities
/// @{
constexpr SDL_Color
rawToComponent(RawColor color, const SDL_PixelFormat* format)
{
  if (format == nullptr) {
    return {
      Uint8(color >> 24), Uint8(color >> 16), Uint8(color >> 8), Uint8(color)};
  }
  SDL_Color c;
  SDL_GetRGBA(color, format, &c.r, &c.g, &c.b, &c.a);
  return c;
}
constexpr RawColor
componentToRaw(SDL_Color color, const SDL_PixelFormat* format)
{
  if (format == nullptr)
    return color.r << 24 | color.g << 16 | color.b << 8 | color.a;
  return SDL_MapRGBA(format, color.r, color.g, color.b, color.a);
}

constexpr std::array<float, 4>
componentToNormalized(SDL_Color color)
{
  return {color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f};
}
constexpr SDL_Color
normalizedToComponent(std::array<float, 4> color)
{
  return {
    Uint8(color[0] * 255.f),
    Uint8(color[1] * 255.f),
    Uint8(color[2] * 255.f),
    Uint8(color[3] * 255.f),
  };
}
///}
} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_COLOR_INCLUDED */
