#ifndef PIXEDIT_SRC_UTILS_COLOR_INCLUDED
#define PIXEDIT_SRC_UTILS_COLOR_INCLUDED

#include <array>
#include <SDL3pp/SDL3pp.h>

#include "Surface.hpp"

namespace pixedit {

using SDL::Color;

/// @brief Raw color type
using RawColor = Uint32;

/// @brief Color conversion utilities
/// @{
constexpr Color
rawToComponent(RawColor color, const Surface& surface)
{
  if (!surface) return SDL::GetColor(color, DEFAULT_FORMAT);
  return SDL::GetColor(color, surface.GetFormat(), surface.GetPalette().get());
}
constexpr RawColor
componentToRaw(Color color, const Surface& surface)
{
  if (surface == nullptr) return SDL::MapColor(DEFAULT_FORMAT, color);
  return surface.MapRGBA(color);
}

constexpr std::array<float, 4>
componentToNormalized(Color color)
{ return {color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f}; }

constexpr Color
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

inline void
setColorIndex(const Surface& surface, int index, Color newColor)
{
  auto palette = surface.GetPalette();
  if (!palette) return;
  palette.SetColors({&newColor, 1}, index);
}
} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_COLOR_INCLUDED */
