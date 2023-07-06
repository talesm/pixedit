#include "PictureView.hpp"
#include <cmath>

namespace pixedit {

void
PictureView::updatePreview(SDL_Renderer* renderer)
{
  if (buffer != newBuffer) { buffer = newBuffer; }
  if (!buffer || !buffer->getSurface()) return;
  if (!scratchEnabled) canvas.setSurface(buffer->getSurface());
  auto createPreview = [renderer, w = buffer->getW(), h = buffer->getH()] {
    auto t = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    return t;
  };
  if (preview) {
    int w, h;
    SDL_QueryTexture(preview, nullptr, nullptr, &w, &h);
    if (w < buffer->getW() || h < buffer->getH()) {
      SDL_DestroyTexture(preview);
      preview = createPreview();
    }
  } else {
    SDL_DestroyTexture(preview);
    preview = createPreview();
  }
  SDL_Rect dstRect{0, 0, buffer->getW(), buffer->getH()};
  SDL_Surface* previewSurface;
  SDL_LockTextureToSurface(preview, &dstRect, &previewSurface);
  SDL_FillRect(previewSurface, nullptr, 0);
  SDL_BlitSurface(buffer->getSurface(), &dstRect, previewSurface, &dstRect);
  if (buffer->hasSelection()) {
    SDL_BlitSurface(buffer->getSelectionSurface(),
                    nullptr,
                    previewSurface,
                    &buffer->getSelectionRect());
  }
  if (scratchEnabled) {
    SDL_BlitSurface(scratch, &dstRect, previewSurface, &dstRect);
  }
  if (glassEnabled) {
    SDL_BlitSurface(glassSurface, &dstRect, previewSurface, &dstRect);
  }
  SDL_UnlockTexture(preview);
  movingMode = false;
  changed = false;
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

float
PictureView::effectiveScale() const
{
  return std::clamp(scale, 1 / 256.f, 256.f);
}

SDL_FPoint
PictureView::effectiveSize() const
{
  if (!buffer || !buffer->getSurface()) { return {0}; }
  float scale = effectiveScale();
  return {
    scale * buffer->getW(),
    scale * buffer->getH(),
  };
}

SDL_FPoint
PictureView::effectiveOffset() const
{
  SDL_FPoint effectiveOffset{0, 0};

  SDL_FPoint viewSize{float(viewport.w), float(viewport.h)};
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
  if (!buffer || !buffer->getSurface()) return;
  float scale = effectiveScale();
  SDL_FPoint scaledSz = {
    scale * buffer->getW(),
    scale * buffer->getH(),
  };
  auto offset = effectiveOffset();
  SDL_Rect srcRect{0, 0, buffer->getW(), buffer->getH()};
  SDL_FRect dstRect{
    viewport.x + offset.x + (viewport.w - scaledSz.x) / 2.f,
    viewport.y + offset.y + (viewport.h - scaledSz.y) / 2.f,
    scaledSz.x,
    scaledSz.y,
  };
  renderCheckerBoard(
    renderer, dstRect, checkerSize, checkerColors[0], checkerColors[1]);
  SDL_RenderCopyF(renderer, preview, &srcRect, &dstRect);
}

void
PictureView::enableScratch(bool enable)
{
  if (!buffer) {
    scratchEnabled = false;
    return;
  }
  scratchEnabled = enable;
  changed = true;
  if (!enable) {
    canvas.setSurface(buffer->getSurface());
    scratch = nullptr;
  } else if (scratch != nullptr && scratch->w >= buffer->getW() &&
             scratch->h >= buffer->getH()) {
    SDL_Rect r{0, 0, buffer->getW(), buffer->getH()};
    SDL_FillRect(scratch, &r, 0);
    canvas.setSurface(scratch, true);
  } else {
    scratch = SDL_CreateRGBSurfaceWithFormat(
      0, buffer->getW(), buffer->getH(), 32, SDL_PIXELFORMAT_ABGR32);
    SDL_assert(scratch);
    canvas.setSurface(scratch, true);
  }
}

void
PictureView::update(SDL_Renderer* renderer)
{
  if (buffer != newBuffer) {
    if (!buffer) {
      buffer = newBuffer;
      updatePreview(renderer);
      return;
    }
    changed = true;
    if (editing && tool) { cancelEdit(); }
    state.left = state.middle = state.right = false; // Clear everything
    state.wheelX = oldState.wheelX;
    state.wheelY = oldState.wheelY;
  }
  if (!buffer || !buffer->getSurface()) return;
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
    bool wasGlassEnabled = glassEnabled;
    glassEnabled = false;
    tool->update(*this, renderer, event);
    if (!changed) glassEnabled = wasGlassEnabled;
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
    scale = effectiveScale();
  } else if (wheelY > 0) {
    scale *= 2;
    scale = effectiveScale();
  }

  oldState = state;
  if (changed) { updatePreview(renderer); }
}

SDL_Point
PictureView::effectivePos() const
{
  auto offset = effectiveOffset();
  auto scale = effectiveScale();
  auto size = effectiveSize();

  float xx = (state.x - viewport.w / 2 - offset.x + size.x / 2.f) / scale;
  float yy = (state.y - viewport.h / 2 - offset.y + size.y / 2.f) / scale;
  return {
    int(xx),
    int(yy),
  };
}

Canvas&
PictureView::getGlassCanvas()
{
  if (!buffer) { return glassCanvas; }
  glassEnabled = true;
  changed = true;

  if (glassSurface != nullptr && glassSurface->w >= buffer->getW() &&
      glassSurface->h >= buffer->getH()) {
    SDL_Rect r{0, 0, buffer->getW(), buffer->getH()};
    SDL_FillRect(glassSurface, &r, 0);
  } else {
    glassSurface = SDL_CreateRGBSurfaceWithFormat(
      0, buffer->getW(), buffer->getH(), 32, SDL_PIXELFORMAT_ABGR32);
    SDL_assert(glassSurface);
  }
  glassCanvas.setSurface(glassSurface, true);
  return glassCanvas;
}

void
PictureView::setSelection(SDL_Surface* surface)
{
  if (!buffer) return;
  if (surface) {
    buffer->setSelection(surface, {0, 0, surface->w, surface->h});
    changed = true;
  } else {
    buffer->clearSelection();
  }
}

} // namespace pixedit
