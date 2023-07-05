#ifndef PIXEDIT_SRC_TOOLS_FREE_HAND_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_FREE_HAND_TOOL_INCLUDED

#include "PictureTool.hpp"
#include "PictureView.hpp"

namespace pixedit {

struct FreeHandTool : PictureTool
{
  bool pressed = false;
  SDL_Point lastPoint;

  void update(PictureView& view,
              SDL_Renderer* renderer,
              PictureEvent event) final
  {
    switch (event) {
    case PictureEvent::LEFT:
      pressed = true;
      lastPoint = view.effectivePos();
      view.canvas | lastPoint;
      view.previewChange();
      break;

    case PictureEvent::NONE:
      if (pressed) {
        auto currPoint = view.effectivePos();
        if (currPoint.x == lastPoint.x && currPoint.y == lastPoint.y) break;
        view.canvas | LineTo(currPoint, lastPoint);
        view.persistChange();
        lastPoint = currPoint;
      }
      break;

    default: pressed = false; break;
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_FREE_HAND_TOOL_INCLUDED */
