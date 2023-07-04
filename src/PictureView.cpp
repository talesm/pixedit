#include "PictureView.hpp"
#include <cmath>

namespace pixedit {

void
PictureView::updatePreview(SDL_Renderer* renderer)
{
  if (!buffer || !buffer->surface) return;
  auto createPreview =
    [renderer, w = buffer->surface->w, h = buffer->surface->h] {
      auto t = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, w, h);
      SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
      return t;
    };
  if (preview) {
    int w, h;
    SDL_QueryTexture(preview, nullptr, nullptr, &w, &h);
    if (w < buffer->surface->w || h < buffer->surface->h) {
      SDL_DestroyTexture(preview);
      preview = createPreview();
    }
  } else {
    SDL_DestroyTexture(preview);
    preview = createPreview();
  }
  SDL_Rect dstRect{0, 0, buffer->surface->w, buffer->surface->h};
  SDL_Surface* previewSurface;
  SDL_LockTextureToSurface(preview, &dstRect, &previewSurface);
  SDL_FillRect(previewSurface, nullptr, 0);
  SDL_BlitSurface(buffer->surface, nullptr, previewSurface, &dstRect);
  SDL_UnlockTexture(preview);
  movingMode = false;
}

static void
renderCheckerBoard(SDL_Renderer* renderer,
                   const SDL_FRect& rect,
                   float squareSize,
                   SDL_Color color1,
                   SDL_Color color2)
{
  SDL_SetRenderDrawColor(renderer, color1.r, color1.g, color1.b, 255);
  SDL_RenderFillRectF(renderer, &rect);
  SDL_SetRenderDrawColor(renderer, color2.r, color2.g, color2.b, 255);
  SDL_FRect rect2 = {rect.x, rect.y, squareSize, squareSize};
  bool oddRow = false;
  bool oddCol = false;
  float x2 = rect.x + rect.w;
  float y2 = rect.y + rect.h;
  for (; rect2.y + rect2.h <= y2; rect2.y += rect2.h) {
    for (; rect2.x + rect2.w <= x2; rect2.x += rect2.w) {
      if (oddCol == oddRow) SDL_RenderFillRectF(renderer, &rect2);
      oddCol = !oddCol;
    }
    if (rect2.x < x2 && oddCol == oddRow) {
      rect2.w = x2 - rect2.x;
      SDL_RenderFillRectF(renderer, &rect2);
      rect2.w = squareSize;
    }
    oddRow = !oddRow;
    oddCol = false;
    rect2.x = rect.x;
  }
  if (rect2.y < y2) {
    rect2.h = y2 - rect2.y;
    for (; rect2.x + rect2.w <= x2; rect2.x += rect2.w) {
      if (oddCol == oddRow) SDL_RenderFillRectF(renderer, &rect2);
      oddCol = !oddCol;
    }
    if (rect2.x < x2 && oddCol == oddRow) {
      rect2.w = x2 - rect2.x;
      SDL_RenderFillRectF(renderer, &rect2);
      rect2.w = squareSize;
    }
  }
}
SDL_FPoint
PictureView::effectiveSize() const
{
  if (!buffer || !buffer->surface) { return {0}; }
  return {
    scale * buffer->surface->w,
    scale * buffer->surface->h,
  };
}

SDL_FPoint
PictureView::effectiveOffset() const
{
  SDL_FPoint effectiveOffset{0, 0};

  SDL_FPoint viewSize{float(viewPort.w), float(viewPort.h)};
  SDL_FPoint bufferSize = effectiveSize();
  if (viewSize.x < bufferSize.x) {
    float delta = (bufferSize.x - viewSize.x) / 2;
    effectiveOffset.x = std::clamp(offset.x, -delta, delta);
  }
  if (viewSize.y < bufferSize.y) {
    float delta = (bufferSize.y - viewSize.y) / 2;
    effectiveOffset.y = std::clamp(offset.y, -delta, delta);
  }

  return effectiveOffset;
}

void
PictureView::render(SDL_Renderer* renderer) const
{
  if (!buffer || !buffer->surface) return;
  SDL_FPoint scaledSz = {
    scale * buffer->surface->w,
    scale * buffer->surface->h,
  };
  auto offset = effectiveOffset();
  SDL_Rect srcRect{0, 0, buffer->surface->w, buffer->surface->h};
  SDL_FRect dstRect{
    viewPort.x + offset.x + (viewPort.w - scaledSz.x) / 2.f,
    viewPort.y + offset.y + (viewPort.h - scaledSz.y) / 2.f,
    scaledSz.x,
    scaledSz.y,
  };
  renderCheckerBoard(
    renderer, dstRect, checkerSize, checkerColors[0], checkerColors[1]);
  SDL_RenderCopyF(renderer, preview, &srcRect, &dstRect);
}

void
PictureView::update(SDL_Renderer* renderer)
{
  auto event = PictureEvent::NONE;
  if (state.left) {
    if (!oldState.left) {
      if (!oldState.right) {
        event = PictureEvent::LEFT;
      } else {
        event = PictureEvent::CANCEL;
      }
    } else if (oldState.right) {
      state.right = true; // To mark both clicked
    } else if (state.right) {
      event = PictureEvent::CANCEL;
    }
  } else if (state.right) {
    if (!oldState.right) {
      if (!oldState.left) {
        event = PictureEvent::RIGHT;
      } else {
        event = PictureEvent::CANCEL;
      }
    } else if (oldState.left) {
      state.left = true;
    }
  } else if (oldState.left != oldState.right) {
    event = PictureEvent::OK;
  }
  if (state.middle) {
    movingMode = true;
  } else if (oldState.middle) {
    movingMode = false;
  } else if (tool != nullptr) {
    tool->update(*this, renderer, event);
  } else if (event == PictureEvent::LEFT) {
    movingMode = true;
  } else if (event != PictureEvent::NONE) {
    movingMode = false;
  }
  if (movingMode) {
    offset.x += state.x - oldState.x;
    offset.y += state.y - oldState.y;
    offset = effectiveOffset();
  }
  int wheelY = state.wheelY - oldState.wheelY;
  if (wheelY < 0) {
    scale /= 2;
  } else if (wheelY > 0) {
    scale *= 2;
  }

  oldState = state;
}
} // namespace pixedit
