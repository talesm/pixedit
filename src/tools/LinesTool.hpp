#ifndef PIXEDIT_SRC_TOOLS_LINES_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_LINES_TOOL_INCLUDED

#include "PictureView.hpp"
#include "primitives/Line.hpp"

namespace pixedit {

struct LinesTool
{
  SDL_Point lastPoint;
  bool multiline = false;

  void operator()(PictureView& view, PictureEvent event)
  {
    switch (event) {
    case PictureEvent::LEFT:
      if (view.isEditing()) { break; }
      view.beginEdit();
      multiline = false;
      lastPoint = view.effectivePos();
      view.canvas | lastPoint;
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
        if (!view.isScratchEnabled()) multiline = !multiline;
        auto currPoint = view.effectivePos();
        view.enableScratch(false);
        view.canvas | OpenLineTo(currPoint, lastPoint);
        if (multiline) {
          lastPoint = currPoint;
          view.previewEdit();
        } else {
          view.endEdit();
        }
      };
      break;

    case PictureEvent::RIGHT:
      if (view.isEditing() && multiline) {
        auto currPoint = view.effectivePos();
        view.enableScratch(false);
        view.canvas | OpenLineTo(currPoint, lastPoint);
        view.endEdit();
      } else {
        view.pickColorUnderMouse();
      }

    default: view.cancelEdit(); break;
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_LINES_TOOL_INCLUDED */
