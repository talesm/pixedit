#ifndef PIXEDIT_SRC_PICTURE_VIEW_INCLUDED
#define PIXEDIT_SRC_PICTURE_VIEW_INCLUDED

#include <SDL.h>
#include "MouseState.hpp"
#include "PictureBuffer.hpp"
#include "PictureTool.hpp"

namespace pixedit {

struct PictureView
{
  PictureBuffer buffer;
  SDL_Rect viewPort;
  SDL_FPoint offset{0};
  float scale{1.f};
  MouseState state{}, oldState{};
  bool movingMode = false;

  PictureTool* tool = nullptr;
  SDL_Texture* preview = nullptr;
  SDL_Color checkerColors[2] = {{200, 200, 200, 255}, {150, 150, 150, 255}};
  int checkerSize = 16;

  void updatePreview(SDL_Renderer* renderer);

  void update(SDL_Renderer* renderer);

  void render(SDL_Renderer* renderer) const;
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_VIEW_INCLUDED */
