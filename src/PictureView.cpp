#include "PictureView.hpp"
#include <cmath>
#include "tools.hpp"
#include "utils/pixel.hpp"
#include "utils/rect.hpp"

namespace pixedit {

void
PictureView::updatePreview(SDL_Renderer* renderer)
{
  changed = false;
  if (!buffer || !buffer->getSurface()) return;
  if (!scratchEnabled) canvas.setSurface(buffer->getSurface());
  auto createPreview =
    [renderer, w = buffer->GetWidth(), h = buffer->GetHeight()] {
      auto t = SDL_CreateTexture(
        renderer, DEFAULT_FORMAT, SDL::TEXTUREACCESS_STREAMING, w, h);
      SDL::SetTextureBlendMode(t, SDL::BLENDMODE_BLEND);
      SDL::SetTextureScaleMode(t, SDL::SCALEMODE_NEAREST);
      return t;
    };
  movingMode = false;
  if (!renderer) { return; }
  if (preview) {
    int w = preview->w, h = preview->h;
    if (w < buffer->GetWidth() || h < buffer->GetHeight()) {
      SDL_DestroyTexture(preview);
      preview = createPreview();
    }
  } else {
    SDL_DestroyTexture(preview);
    preview = createPreview();
  }
  SDL_Rect dstRect{0, 0, buffer->GetWidth(), buffer->GetHeight()};
  Surface previewSurface;
  {
    SDL_Surface* temp;
    SDL_LockTextureToSurface(preview, &dstRect, &temp);
    previewSurface = Surface::Borrow(temp);
  }

  previewSurface.FillRect(dstRect, 0);
  previewSurface.Blit(buffer->getSurface(), {}, {});
  if (buffer->hasSelection()) {
    auto selection = buffer->getSelectionSurface();
    auto mask = buffer->getSelectionMask();
    if (mask && !transparent) {
      mask.SetColorKey(1);
      setColorIndex(mask, 1, canvas.getColorB());
      selection.Blit(mask, {}, {});
      selection.SetColorKey(selection.MapRGBA(canvas.getColorB()));
    }
    previewSurface.BlitScaled(buffer->getSelectionSurface(),
                              {},
                              buffer->getSelectionRect(),
                              SDL::SCALEMODE_NEAREST);
    if (mask && !transparent) selection.ClearColorKey();
  }
  if (scratchEnabled) { previewSurface.Blit(scratch, {}, {}); }
  SDL_UnlockTexture(preview);
}

static void
renderCheckerBoard(SDL_Renderer* renderer,
                   const SDL_FRect& rect,
                   float squareSize,
                   SDL_Color color1,
                   SDL_Color color2)
{
  SDL_SetRenderDrawColor(renderer, color1.r, color1.g, color1.b, 255);
  SDL_RenderFillRect(renderer, &rect);
  SDL_SetRenderDrawColor(renderer, color2.r, color2.g, color2.b, 255);
  SDL_FRect rect2 = {rect.x, rect.y, squareSize, squareSize};
  bool oddRow = false;
  bool oddCol = false;
  float x2 = rect.x + rect.w;
  float y2 = rect.y + rect.h;
  for (; rect2.y + rect2.h <= y2; rect2.y += rect2.h) {
    for (; rect2.x + rect2.w <= x2; rect2.x += rect2.w) {
      if (oddCol == oddRow) SDL_RenderFillRect(renderer, &rect2);
      oddCol = !oddCol;
    }
    if (rect2.x < x2 && oddCol == oddRow) {
      rect2.w = x2 - rect2.x;
      SDL_RenderFillRect(renderer, &rect2);
      rect2.w = squareSize;
    }
    oddRow = !oddRow;
    oddCol = false;
    rect2.x = rect.x;
  }
  if (rect2.y < y2) {
    rect2.h = y2 - rect2.y;
    for (; rect2.x + rect2.w <= x2; rect2.x += rect2.w) {
      if (oddCol == oddRow) SDL_RenderFillRect(renderer, &rect2);
      oddCol = !oddCol;
    }
    if (rect2.x < x2 && oddCol == oddRow) {
      rect2.w = x2 - rect2.x;
      SDL_RenderFillRect(renderer, &rect2);
      rect2.w = squareSize;
    }
  }
}

float
PictureView::effectiveScale() const
{ return std::clamp(scale, 1 / 256.f, 256.f); }

SDL_FPoint
PictureView::effectiveSize() const
{
  if (!buffer || !buffer->getSurface()) { return {0}; }
  float scale = effectiveScale();
  return {
    scale * buffer->GetWidth(),
    scale * buffer->GetHeight(),
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
    scale * buffer->GetWidth(),
    scale * buffer->GetHeight(),
  };
  auto offset = effectiveOffset();
  SDL_FRect srcRect{
    0, 0, float(buffer->GetWidth()), float(buffer->GetHeight())};
  SDL_FRect dstRect{
    viewport.x + offset.x + (viewport.w - scaledSz.x) / 2.f,
    viewport.y + offset.y + (viewport.h - scaledSz.y) / 2.f,
    scaledSz.x,
    scaledSz.y,
  };
  renderCheckerBoard(
    renderer, dstRect, checkerSize, checkerColors[0], checkerColors[1]);
  SDL_RenderTexture(renderer, preview, &srcRect, &dstRect);
  if (grid && scale >= 3) {
    SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
    float yLimit = dstRect.y + scaledSz.y;
    float xLimit = dstRect.x + scaledSz.x;
    for (float yy = dstRect.y + scale - 1; yy < yLimit; yy += scale) {
      SDL_RenderLine(renderer, dstRect.x, yy, xLimit - 1, yy);
    }
    for (float xx = dstRect.x + scale - 1; xx < xLimit; xx += scale) {
      SDL_RenderLine(renderer, xx, dstRect.y, xx, yLimit - 1);
    }
  }
}

void
PictureView::enableScratch(bool enable)
{
  if (!buffer) {
    scratchEnabled = false;
    return;
  }
  if (!(enable || scratchEnabled)) { return; }
  scratchEnabled = enable;
  changed = true;
  if (!enable) {
    canvas.setSurface(buffer->getSurface());
    scratch.Destroy();
  } else if (scratch && scratch.GetWidth() >= buffer->GetWidth() &&
             scratch.GetHeight() >= buffer->GetHeight()) {
    scratch.FillRect(Rect{0, 0, buffer->GetWidth(), buffer->GetHeight()}, 0);
    canvas.setSurface(scratch);
  } else {
    scratch = Surface(buffer->getSize(), DEFAULT_FORMAT);
    SDL_assert(scratch);
    canvas.setSurface(scratch);
  }
}

inline std::optional<IdRef>
adjustNextToolId(IdRef toolId)
{
  if (getTool(toolId).flags & ToolDescription::ENABLE_SELECTION) { return {}; }
  for (auto& tool : getTools()) {
    if (tool.flags & ToolDescription::ENABLE_SELECTION) { return Id{tool.id}; }
  }
  return {};
}

void
PictureView::update(SDL_Renderer* renderer)
{
  if (nextToolId) {
    if (tool) { tool(*this, PictureEvent::DETACHED); }
    auto& toolDesc = getTool(*nextToolId);
    toolId = toolDesc.id;
    nextToolId.reset();
    tool = toolDesc.build();
  }
  if (buffer != newBuffer) {
    if (!buffer) {
      buffer = newBuffer;
      updatePreview(renderer);
      return;
    }
    changed = true;
    if (tool) {
      if (editing) { cancelEdit(); }
      tool(*this, PictureEvent::RESET);
    }
    buffer = newBuffer;
    if (buffer->hasSelection()) { nextToolId = adjustNextToolId(toolId); }
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
    tool(*this, event);
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

void
PictureView::setTransparent(bool value)
{
  transparent = value;
  if (!buffer || !buffer->hasSelection()) { return; }
  auto selection = buffer->getSelectionSurface();
  if (transparent) {
    if (selection.GetFormat().IsAlpha()) {
      selection.SetBlendMode(SDL::BLENDMODE_BLEND);
    } else {
      selection.SetColorKey(selection.MapRGBA(canvas.getColorB()));
    }
  } else {
    if (selection.GetFormat().IsAlpha()) {
      selection.SetBlendMode(SDL::BLENDMODE_NONE);
    } else {
      selection.ClearColorKey();
    }
  }
  changed = true;
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

void
PictureView::setSelection(Surface surface)
{
  if (!buffer) return;
  if (surface) {
    auto offset = effectiveOffset();
    auto size = effectiveSize();
    auto scale = effectiveScale();
    int xx = 0;
    int yy = 0;
    if (size.x > viewport.w) {
      xx = ceil((size.x - viewport.w) / 2 / scale - offset.x);
    }
    if (size.y > viewport.h) {
      yy = ceil((size.y - viewport.h) / 2 / scale - offset.y);
    }
    buffer->setSelection(surface,
                         {xx, yy, surface.GetWidth(), surface.GetHeight()});
    setTransparent(transparent);
    changed = true;
    nextToolId = adjustNextToolId(toolId);
  } else if (buffer->hasSelection()) {
    buffer->clearSelection();
    changed = true;
  }
}

void
PictureView::persistSelection()
{
  if (!buffer || !buffer->hasSelection()) return;
  changed = true;
  buffer->persistSelection();
}

void
PictureView::pickColorUnderMouse()
{
  if (!buffer) return;
  auto surface = buffer->getSurface();
  auto pos = effectivePos();
  if (pos.x < 0 || pos.y < 0 || pos.x >= surface.GetWidth() ||
      pos.y >= surface.GetHeight())
    return;
  canvas | RawColorA{getPixelAt(surface, pos.x, pos.y)};
}

} // namespace pixedit
