#ifndef PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED

#include "renderSelection.hpp"

namespace pixedit {

struct SelectionHandTool
{
  SDL_Point lastPoint;
  bool moving = false;

  void operator()(PictureView& view, PictureEvent event)
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
        renderSelection(view.getGlassCanvas(), rect);
        moving = false;
      }
      break;
    case PictureEvent::RESET: break;

    default: view.persistSelection(); break;
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED */
