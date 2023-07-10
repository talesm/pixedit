#ifndef PIXEDIT_SRC_TOOLS_FLOOD_FILL_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_FLOOD_FILL_TOOL_INCLUDED

#include <vector>
#include "PictureTool.hpp"
#include "PictureView.hpp"
#include "utils/surface.hpp"

namespace pixedit {

inline void
floodFill(SDL_Surface* surface, const SDL_Point& p, RawColor color)
{
  if (p.x < 0 || p.y < 0 || p.x >= surface->w || p.y >= surface->h) { return; }
  auto prevColor = getPixelAt(surface, p.x, p.y);
  if (prevColor == color) { return; }
  std::vector<SDL_Point> stack{p};
  while (!stack.empty()) {
    auto p = stack.back();
    stack.pop_back();

    auto pixelPtr = pixelAt(surface, p.x, p.y);
    auto c = getPixel(pixelPtr, surface->format->BytesPerPixel);
    if (c != prevColor) continue;

    setPixel(pixelPtr, color, surface->format->BytesPerPixel);

    if (p.x > 0) stack.emplace_back(p.x - 1, p.y);
    if (p.x < surface->w - 1) stack.emplace_back(p.x + 1, p.y);
    if (p.y > 0) stack.emplace_back(p.x, p.y - 1);
    if (p.y < surface->h - 1) stack.emplace_back(p.x, p.y + 1);
  }
}

struct FloodFillTool : PictureTool
{
  void update(PictureView& view, PictureEvent event) final
  {
    if (event == PictureEvent::LEFT) {
      view.beginEdit();
      floodFill(view.getBuffer()->getSurface(),
                view.effectivePos(),
                view.canvas.getRawColorA());
      view.endEdit();
    } else if (event == PictureEvent::RIGHT) {
      view.pickColorUnderMouse();
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_FLOOD_FILL_TOOL_INCLUDED */
