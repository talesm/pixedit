#include "Canvas.hpp"

namespace pixedit {

void
Canvas::updatePreview(SDL_Renderer* renderer)
{
  SDL_DestroyTexture(preview);
  preview = SDL_CreateTextureFromSurface(renderer, buffer.surface);
}

static void
renderCheckerBoard(SDL_Renderer* renderer,
                   const SDL_FRect& rect,
                   SDL_Color color1,
                   SDL_Color color2)
{
  SDL_SetRenderDrawColor(renderer, color1.r, color1.g, color1.b, 255);
  SDL_RenderFillRectF(renderer, &rect);
  SDL_SetRenderDrawColor(renderer, color2.r, color2.g, color2.b, 255);
  SDL_FRect rect2 = {rect.x, rect.y, 16, 16};
  bool oddRow = false;
  bool oddCol = false;
  for (; rect2.y + rect2.h <= rect.y + rect.h; rect2.y += rect2.h) {
    for (; rect2.x + rect2.w <= rect.x + rect.w; rect2.x += rect2.w) {
      if (oddCol == oddRow) SDL_RenderFillRectF(renderer, &rect2);
      oddCol = !oddCol;
    }
    oddRow = !oddRow;
    oddCol = false;
    rect2.x = rect.x;
  }
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
  renderCheckerBoard(renderer, rect, checkerColors[0], checkerColors[1]);
  SDL_RenderCopyF(renderer, preview, nullptr, &rect);
}

} // namespace pixedit
