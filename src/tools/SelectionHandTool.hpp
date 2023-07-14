#ifndef PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED

#include "renderSelection.hpp"

namespace pixedit {

struct SelectionHandTool
{
  SDL_Point lastPoint;
  bool moving = false;
  bool scalingVertical = false;
  bool scalingHorizontal = false;

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
        if (rect.w > 1 && lastPoint.x == rect.x + rect.w - 1) {
          scalingHorizontal = true;
        }
        if (rect.w > 1 && lastPoint.y == rect.y + rect.h - 1) {
          scalingVertical = true;
        }
        if (!scalingHorizontal && !scalingVertical) { moving = true; }
        renderSelection(view.getGlassCanvas(), rect, buffer.getSelectionMask());
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
        auto currPoint = view.effectivePos();
        if (scalingHorizontal) { rect.w += currPoint.x - lastPoint.x; }
        if (scalingVertical) { rect.h += currPoint.y - lastPoint.y; }
        lastPoint = currPoint;
        renderSelection(view.getGlassCanvas(), rect, buffer.getSelectionMask());
      }
      break;

    case PictureEvent::OK:
      if (moving) {
        renderSelection(view.getGlassCanvas(), rect, buffer.getSelectionMask());
        moving = false;
      } else if (scalingHorizontal || scalingVertical) {
        scalingHorizontal = scalingVertical = false;
      }
      break;
    case PictureEvent::RESET: break;

    default: view.persistSelection(); break;
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED */
