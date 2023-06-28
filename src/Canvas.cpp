#include "Canvas.hpp"

namespace pixedit {

void
Canvas::updatePreview(SDL_Renderer* renderer)
{
  SDL_DestroyTexture(preview);
  preview = SDL_CreateTextureFromSurface(renderer, buffer.surface);
}

void
Canvas::render(SDL_Renderer* renderer) const
{
  SDL_FRect rect{
    viewPort.x + (viewPort.w - buffer.surface->w) / 2.f,
    viewPort.y + (viewPort.h - buffer.surface->h) / 2.f,
    buffer.surface->w,
    buffer.surface->h,
  };
  SDL_RenderCopyF(renderer, preview, nullptr, &rect);
}

} // namespace pixedit
