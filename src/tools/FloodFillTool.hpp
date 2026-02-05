#ifndef PIXEDIT_SRC_TOOLS_FLOOD_FILL_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_FLOOD_FILL_TOOL_INCLUDED

#include <vector>
#include "PictureView.hpp"
#include "utils/pixel.hpp"

namespace pixedit {

inline void
floodFill(Surface surface, const SDL_Point& p, RawColor color)
{
  const int WW = surface.GetWidth();
  const int HH = surface.GetHeight();
  if (p.x < 0 || p.y < 0 || p.x >= WW || p.y >= HH) { return; }

  auto BPP = surface.GetFormat().GetBytesPerPixel();
  auto prevColor = getPixelAt(surface, p.x, p.y);
  if (prevColor == color) { return; }
  std::vector<SDL_Point> stack{p};

  while (!stack.empty()) {
    auto pp = stack.back();
    stack.pop_back();

    auto pixelPtr = pixelAt(surface, pp.x, pp.y);
    auto c = getPixel(pixelPtr, BPP);
    if (c != prevColor) continue;
    setPixel(pixelPtr, color, BPP);

    if (pp.x > 0) stack.emplace_back(pp.x - 1, pp.y);
    if (pp.x < WW - 1) stack.emplace_back(pp.x + 1, pp.y);
    if (pp.y > 0) stack.emplace_back(pp.x, pp.y - 1);
    if (pp.y < HH - 1) stack.emplace_back(pp.x, pp.y + 1);
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
