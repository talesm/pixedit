#ifndef PIXEDIT_SRC_TOOLS_LINES_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_LINES_TOOL_INCLUDED

#include "PictureTool.hpp"
#include "PictureView.hpp"

namespace pixedit {

struct LinesTool : PictureTool
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
      view.enableScratch();
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
        auto currPoint = view.effectivePos();
        view.enableScratch(false);
        view.canvas | LineTo(currPoint, lastPoint);
        view.endEdit();
      };
      break;
    default: view.cancelEdit(); break;
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_LINES_TOOL_INCLUDED */
