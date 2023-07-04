#ifndef PIXEDIT_SRC_PICTURE_VIEW_INCLUDED
#define PIXEDIT_SRC_PICTURE_VIEW_INCLUDED

#include <memory>
#include <SDL.h>
#include "MouseState.hpp"
#include "PictureBuffer.hpp"
#include "PictureTool.hpp"

namespace pixedit {

struct PictureView
{
  SDL_Rect viewPort;
  std::shared_ptr<PictureBuffer> buffer;
  SDL_FPoint offset{0};
  float scale{1.f};
  MouseState state{}, oldState{};
  bool movingMode = false;

  PictureTool* tool = nullptr;
  SDL_Texture* preview = nullptr;
  SDL_Color checkerColors[2] = {{200, 200, 200, 255}, {150, 150, 150, 255}};
  int checkerSize = 16;

  PictureView(const SDL_Rect& viewPort)
    : viewPort(viewPort)
  {
  }

  void updatePreview(SDL_Renderer* renderer);

  void update(SDL_Renderer* renderer);

  void render(SDL_Renderer* renderer) const;

  SDL_FPoint effectiveSize() const;

  SDL_FPoint effectiveOffset() const;
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_VIEW_INCLUDED */
