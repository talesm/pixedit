#ifndef PIXEDIT_SRC_TOOLS_ZOOM_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_ZOOM_TOOL_INCLUDED

#include "PictureView.hpp"

namespace pixedit {

struct ZoomTool
{
  void operator()(PictureView& view, PictureEvent event)
  {
    if (event == PictureEvent::LEFT) { view.setScale(view.scale * 2); }
    if (event == PictureEvent::RIGHT) { view.setScale(view.scale / 2); }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_ZOOM_TOOL_INCLUDED */
