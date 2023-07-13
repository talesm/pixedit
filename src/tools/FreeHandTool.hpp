#ifndef PIXEDIT_SRC_TOOLS_FREE_HAND_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_FREE_HAND_TOOL_INCLUDED

#include "PictureView.hpp"
#include "primitives/Line.hpp"

namespace pixedit {

struct FreeHandTool
{
  SDL_Point lastPoint;

  void operator()(PictureView& view, PictureEvent event)
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
    case PictureEvent::RIGHT: view.pickColorUnderMouse(); break;
    default: view.cancelEdit(); break;
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_FREE_HAND_TOOL_INCLUDED */
