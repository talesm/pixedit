#ifndef PIXEDIT_SRC_TOOLS_SELECTION_RECT_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_SELECTION_RECT_TOOL_INCLUDED

#include "SelectionHandTool.hpp"
#include "renderSelection.hpp"
#include "utils/cutoutSurface.hpp"
#include "utils/rect.hpp"

namespace pixedit {

struct SelectionRectTool
{
  SelectionHandTool handTool;
  SDL_Point lastPoint;

  void operator()(PictureView& view, PictureEvent event)
  {
    auto& buffer = *view.getBuffer();
    if (buffer.hasSelection()) {
      handTool(view, event);
      return;
    }
    switch (event) {
    case PictureEvent::LEFT:
      view.beginEdit();
      lastPoint = view.effectivePos();
      view.enableScratch();
      renderSelection(view.canvas, {lastPoint.x, lastPoint.y, 1, 1});
      break;

    case PictureEvent::NONE:
      if (view.isEditing()) {
        auto currPoint = view.effectivePos();
        if (currPoint.x == lastPoint.x && currPoint.y == lastPoint.y) break;
        SDL_Rect rect = intersectFromOrigin(
          Rect::fromPoints(currPoint, lastPoint), buffer.getSize());
        view.enableScratch();
        renderSelection(view.canvas, rect, true);
      }
      break;
    case PictureEvent::OK:
      if (view.isEditing()) {
        auto currPoint = view.effectivePos();
        SDL_Rect rect = intersectFromOrigin(
          Rect::fromPoints(currPoint, lastPoint), buffer.getSize());
        view.cancelEdit();
        buffer.setSelection(cutoutSurface(buffer.getSurface(), rect, 0), rect);
        handTool(view, PictureEvent::RESET);
      };
      break;
    default: view.cancelEdit(); break;
    }
  }
};
}

#endif /* PIXEDIT_SRC_TOOLS_SELECTION_RECT_TOOL_INCLUDED */
