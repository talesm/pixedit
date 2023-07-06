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

  void update(PictureView& view,
              SDL_Renderer* renderer,
              PictureEvent event) final
  {
    switch (event) {
    case PictureEvent::LEFT:
      view.beginEdit();
      lastPoint = view.effectivePos();
      view.canvas | lastPoint;
      view.previewEdit();
      break;

    case PictureEvent::NONE:
      if (view.isEditing()) {
        auto currPoint = view.effectivePos();
        if (currPoint.x == lastPoint.x && currPoint.y == lastPoint.y) break;
        view.canvas | OpenLineTo(currPoint, lastPoint);
        view.previewEdit();
        lastPoint = currPoint;
      }
      break;
    case PictureEvent::OK: view.endEdit(); break;
    default: view.cancelEdit(); break;
    }
    renderSelection(view.getGlassCanvas(), {0, 0, 10, 10});
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED */
