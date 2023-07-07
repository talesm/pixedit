#ifndef PIXEDIT_SRC_TOOLS_RECT_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_RECT_TOOL_INCLUDED

#include "PictureTool.hpp"
#include "PictureView.hpp"

namespace pixedit {

struct RectTool : PictureTool
{
  bool outline = false;
  SDL_Point lastPoint;

  RectTool(bool outline)
    : outline(outline)
  {
  }

  void update(PictureView& view, PictureEvent event) final
  {
    switch (event) {
    case PictureEvent::LEFT:
      view.beginEdit();
      lastPoint = view.effectivePos();
      view.enableScratch();
      view.canvas | lastPoint;
      view.previewEdit();
      break;

    case PictureEvent::NONE:
      if (view.isEditing()) {
        auto currPoint = view.effectivePos();
        if (currPoint.x == lastPoint.x && currPoint.y == lastPoint.y) break;
        view.enableScratch();
        if (outline) {
          view.canvas | OutlineRectTo(currPoint, lastPoint);
        } else {
          view.canvas | FillRectTo(currPoint, lastPoint);
        }
        view.previewEdit();
      }
      break;
    case PictureEvent::OK:
      if (view.isEditing()) {
        auto currPoint = view.effectivePos();
        view.enableScratch(false);
        if (outline) {
          view.canvas | OutlineRectTo(currPoint, lastPoint);
        } else {
          view.canvas | FillRectTo(currPoint, lastPoint);
        }
        view.endEdit();
      };
      break;
    default: view.cancelEdit(); break;
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_RECT_TOOL_INCLUDED */
