#ifndef PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED

#include "PictureTool.hpp"

namespace pixedit {

constexpr SDL_Color selectedColorA{0, 0, 128, 255};
constexpr SDL_Color selectedColorB{255, 255, 128, 255};

inline void
renderSelection(Canvas& canvas, SDL_Rect rect)
{
  // Backup
  auto bkpColorA = canvas.getRawColorA();
  auto bkpColorB = canvas.getRawColorB();

  canvas | ColorA{selectedColorA} | ColorB{selectedColorB} |
    patterns::CHECKERED_4;

  canvas | OutlineRect{rect};

  // Restore
  canvas | RawColorA{bkpColorA} | RawColorB{bkpColorB} | patterns::SOLID;
}

struct SelectionHandTool : PictureTool
{
  SDL_Point lastPoint;
  bool moving = false;

  void update(PictureView& view,
              SDL_Renderer* renderer,
              PictureEvent event) final
  {
    auto& buffer = *view.getBuffer();
    if (!buffer.hasSelection()) {
      moving = false;
      return;
    }
    auto& rect = buffer.getSelectionRect();
    switch (event) {
    case PictureEvent::LEFT:
      lastPoint = view.effectivePos();
      if (SDL_PointInRect(&lastPoint, &rect)) {
        moving = true;
        renderSelection(view.getGlassCanvas(), rect);
      } else {
        view.persistSelection();
      }
      break;

    case PictureEvent::NONE:
      if (moving) {
        auto currPoint = view.effectivePos();
        if (currPoint.x == lastPoint.x && currPoint.y == lastPoint.y) break;
        auto& rect = buffer.getSelectionRect();
        rect.x += currPoint.x - lastPoint.x;
        rect.y += currPoint.y - lastPoint.y;
        lastPoint = currPoint;
        view.previewEdit();
      } else {
        renderSelection(view.getGlassCanvas(), rect);
      }
      break;

    case PictureEvent::OK:
      if (moving) {
        renderSelection(view.getGlassCanvas(), buffer.getSelectionRect());
        moving = false;
      }
      break;

    default: view.persistSelection(); break;
    }
  }

  bool acceptsSelection() const final { return true; }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED */
