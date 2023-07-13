#ifndef PIXEDIT_SRC_TOOLS_FLOOD_FILL_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_FLOOD_FILL_TOOL_INCLUDED

#include <vector>
#include "PictureView.hpp"
#include "utils/pixel.hpp"

namespace pixedit {

inline void
floodFill(Surface surface, const SDL_Point& p, RawColor color)
{
  const int WW = surface.getW();
  const int HH = surface.getH();
  if (p.x < 0 || p.y < 0 || p.x >= WW || p.y >= HH) { return; }

  auto BPP = surface.getFormat()->BytesPerPixel;
  auto prevColor = surface.getPixel(p.x, p.y);
  if (prevColor == color) { return; }
  std::vector<SDL_Point> stack{p};

  while (!stack.empty()) {
    auto p = stack.back();
    stack.pop_back();

    auto pixelPtr = surface.pixel(p.x, p.y);
    auto c = getPixel(pixelPtr, BPP);
    if (c != prevColor) continue;
    setPixel(pixelPtr, color, BPP);

    if (p.x > 0) stack.emplace_back(p.x - 1, p.y);
    if (p.x < WW - 1) stack.emplace_back(p.x + 1, p.y);
    if (p.y > 0) stack.emplace_back(p.x, p.y - 1);
    if (p.y < HH - 1) stack.emplace_back(p.x, p.y + 1);
  }
}

struct FloodFillTool
{
  void operator()(PictureView& view, PictureEvent event)
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
