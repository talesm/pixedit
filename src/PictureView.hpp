#ifndef PIXEDIT_SRC_PICTURE_VIEW_INCLUDED
#define PIXEDIT_SRC_PICTURE_VIEW_INCLUDED

#include <memory>
#include <SDL.h>
#include "Canvas.hpp"
#include "MouseState.hpp"
#include "PictureBuffer.hpp"
#include "PictureTool.hpp"

namespace pixedit {

class PictureView
{
  SDL_Rect viewport;
  std::shared_ptr<PictureBuffer> buffer;
  MouseState oldState{};
  SDL_Texture* preview = nullptr;

public:
  SDL_FPoint offset{0};
  float scale{1.f};
  MouseState state{};
  bool movingMode = false;

  Canvas canvas;
  PictureTool* tool = nullptr;
  SDL_Color checkerColors[2] = {{200, 200, 200, 255}, {150, 150, 150, 255}};
  int checkerSize = 16;

  PictureView(const SDL_Rect& viewport)
    : viewport(viewport)
  {
  }

  void updatePreview(SDL_Renderer* renderer);

  void update(SDL_Renderer* renderer);

  void render(SDL_Renderer* renderer) const;

  float effectiveScale() const;

  SDL_FPoint effectiveSize() const;

  SDL_FPoint effectiveOffset() const;

  float setScale(float value)
  {
    scale = value;
    return scale = effectiveScale();
  }

  SDL_Point effectivePos() const;

  constexpr const SDL_Rect& getViewport() { return viewport; }

  void setViewport(const SDL_Rect& value) { viewport = value; }

  const std::shared_ptr<PictureBuffer>& getBuffer() const { return buffer; }

  void setBuffer(const std::shared_ptr<PictureBuffer>& value)
  {
    if (buffer == value) return;
    buffer = value;
  }

  constexpr const MouseState& getState() const { return state; }
  constexpr const MouseState& getOldState() const { return oldState; }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_VIEW_INCLUDED */
