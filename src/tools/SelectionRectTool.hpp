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
  bool selecting = false;
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
      selecting = true;
      lastPoint = view.effectivePos();
      renderSelection(view.getGlassCanvas(), {lastPoint.x, lastPoint.y, 1, 1});
      break;

    case PictureEvent::NONE:
      if (selecting) {
        auto currPoint = view.effectivePos();
        if (currPoint.x == lastPoint.x && currPoint.y == lastPoint.y) break;
        SDL_Rect rect = intersectFromOrigin(
          Rect::fromPoints(currPoint, lastPoint), buffer.getSize());
        renderSelection(view.getGlassCanvas(), rect, true);
      }
      break;
    case PictureEvent::OK:
      if (selecting) {
        selecting = false;
        auto currPoint = view.effectivePos();
        SDL_Rect rect = intersectFromOrigin(
          Rect::fromPoints(currPoint, lastPoint), buffer.getSize());
        renderSelection(view.getGlassCanvas(), rect);
        buffer.setSelection(cutoutSurface(buffer.getSurface(), rect, 0), rect);
      };
      break;
    default:
      selecting = false;
      view.enableGlass(false);
      break;
    }
  }
};
}

#endif /* PIXEDIT_SRC_TOOLS_SELECTION_RECT_TOOL_INCLUDED */
