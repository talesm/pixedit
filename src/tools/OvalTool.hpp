#ifndef PIXEDIT_SRC_TOOLS_OVAL_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_OVAL_TOOL_INCLUDED

#include "PictureView.hpp"
#include "primitives/Oval.hpp"

namespace pixedit {

struct OvalTool
{
  bool outline = false;
  SDL_Point lastPoint;

  OvalTool(bool outline)
    : outline(outline)
  {
  }

  void operator()(PictureView& view, PictureEvent event)
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
          view.canvas | OutlineOvalTo(currPoint, lastPoint);
        } else {
          view.canvas | FillOvalTo(currPoint, lastPoint);
        }
        view.previewEdit();
      }
      break;
    case PictureEvent::OK:
      if (view.isEditing()) {
        auto currPoint = view.effectivePos();
        view.enableScratch(false);
        if (outline) {
          view.canvas | OutlineOvalTo(currPoint, lastPoint);
        } else {
          view.canvas | FillOvalTo(currPoint, lastPoint);
        }
        view.endEdit();
      };
      break;

    case PictureEvent::RIGHT: view.pickColorUnderMouse(); break;
    default: view.cancelEdit(); break;
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_OVAL_TOOL_INCLUDED */
