#ifndef PIXEDIT_SRC_TOOLS_FREE_HAND_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_FREE_HAND_TOOL_INCLUDED

#include "PictureTool.hpp"
#include "PictureView.hpp"

namespace pixedit {

struct FreeHandTool : PictureTool
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
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_FREE_HAND_TOOL_INCLUDED */
