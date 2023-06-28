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
  SDL_FPoint scaledSz = {
    scale * buffer.surface->w,
    scale * buffer.surface->h,
  };
  SDL_FRect rect{
    viewPort.x + offset.x + (viewPort.w - scaledSz.x) / 2.f,
    viewPort.y + offset.y + (viewPort.h - scaledSz.y) / 2.f,
    scaledSz.x,
    scaledSz.y,
  };
  SDL_RenderCopyF(renderer, preview, nullptr, &rect);
}

} // namespace pixedit
