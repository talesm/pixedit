#ifndef PIXEDIT_SRC_TOOLS_ZOOM_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_ZOOM_TOOL_INCLUDED

#include "PictureTool.hpp"
#include "PictureView.hpp"

namespace pixedit {

struct ZoomTool : PictureTool
{
  void update(PictureView& view,
              SDL_Renderer* renderer,
              PictureEvent event) final
  {
    if (event == PictureEvent::LEFT) { view.setScale(view.scale * 2); }
    if (event == PictureEvent::RIGHT) { view.setScale(view.scale / 2); }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_ZOOM_TOOL_INCLUDED */
