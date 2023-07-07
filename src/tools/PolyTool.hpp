#ifndef PIXEDIT_SRC_TOOLS_POLY_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_POLY_TOOL_INCLUDED

#include "PictureTool.hpp"
#include "PictureView.hpp"

namespace pixedit {

struct PolyTool : PictureTool
{
  bool outline;
  SDL_Point firstPoint;
  SDL_Point lastPoint;
  bool firstClick = false;

  PolyTool(bool outline)
    : outline(outline)
  {
  }

  void update(PictureView& view, PictureEvent event) final
  {
    switch (event) {
    case PictureEvent::LEFT:
      if (view.isEditing()) {
        if (!view.isScratchEnabled()) { break; }
        auto currPoint = view.effectivePos();
        view.enableScratch(false);
        view.canvas | OpenLineTo{currPoint, lastPoint};
        lastPoint = currPoint;
      } else {
        view.beginEdit();
        lastPoint = view.effectivePos();
        view.canvas | lastPoint;
        firstPoint = lastPoint;
      }
      firstClick = true;
      view.previewEdit();
      break;

    case PictureEvent::NONE:
      if (view.isEditing()) {
        auto currPoint = view.effectivePos();
        if (currPoint.x == lastPoint.x && currPoint.y == lastPoint.y) break;
        view.enableScratch();
        view.canvas | OpenLineTo(currPoint, lastPoint);
        view.previewEdit();
      }
      break;

    case PictureEvent::OK:
      if (view.isEditing()) {
        if (firstClick) {
          firstClick = false;
        } else {
          view.enableScratch(false);
          view.canvas | OpenLineTo(lastPoint, firstPoint);
          view.endEdit();
        }
      };
      break;

    case PictureEvent::RIGHT:
      if (view.isEditing()) {
        auto currPoint = view.effectivePos();
        view.enableScratch(false);
        view.canvas | OpenLineTo(currPoint, lastPoint);
        view.canvas | OpenLineTo(firstPoint, currPoint);
        view.endEdit();
      }
      break;

    default: view.cancelEdit(); break;
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_POLY_TOOL_INCLUDED */
