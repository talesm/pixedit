#ifndef PIXEDIT_SRC_CANVAS_HPP_INCLUDED
#define PIXEDIT_SRC_CANVAS_HPP_INCLUDED

#include <SDL.h>
#include "Buffer.hpp"

namespace pixedit {

struct BufferView
{
  Buffer buffer;
  SDL_Rect viewPort;
  SDL_FPoint offset{0};
  float scale{1.f};

  SDL_Texture* preview = nullptr;
  SDL_Color checkerColors[2] = {{200, 200, 200, 255}, {150, 150, 150, 255}};
  int checkerSize = 16;

  void updatePreview(SDL_Renderer* renderer);

  void render(SDL_Renderer* renderer) const;
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_CANVAS_HPP_INCLUDED */
