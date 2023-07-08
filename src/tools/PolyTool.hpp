#ifndef PIXEDIT_SRC_TOOLS_POLY_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_POLY_TOOL_INCLUDED

#include <vector>
#include "PictureTool.hpp"
#include "PictureView.hpp"
#include "primitives/Poly.hpp"
namespace pixedit {

struct PolyTool : PictureTool
{
  bool outline;
  std::vector<SDL_Point> points;
  bool moved = false;

  PolyTool(bool outline)
    : outline(outline)
  {
  }

  void update(PictureView& view, PictureEvent event) final
  {
    switch (event) {
    case PictureEvent::LEFT:
      if (view.isEditing()) {
        if (!moved) { break; }
        points.emplace_back(view.effectivePos());
      } else {
        view.beginEdit();
        view.enableScratch();
        points.emplace_back(view.effectivePos());
        render(view.canvas, true);
      }
      moved = true;
      view.previewEdit();
      break;

    case PictureEvent::NONE:
      if (view.isEditing()) {
        auto currPoint = view.effectivePos();
        auto& lastPoint = points.back();
        if (currPoint.x == lastPoint.x && currPoint.y == lastPoint.y) {
          view.enableScratch();
          render(view.canvas, true);
          break;
        }
        moved = true;
        view.enableScratch();
        render(view.canvas, true);
        view.canvas | LineTo{lastPoint, currPoint};
        view.previewEdit();
      }
      break;

    case PictureEvent::OK:
      if (view.isEditing()) {
        if (moved) {
          moved = false;
        } else {
          view.enableScratch(false);
          render(view.canvas, false);
          view.endEdit();
          points.clear();
        }
      };
      break;

    case PictureEvent::RIGHT:
      if (view.isEditing()) {
        points.emplace_back(view.effectivePos());
        view.enableScratch(false);
        render(view.canvas, false);
        view.endEdit();
        points.clear();
      }
      break;

    default: view.cancelEdit(); break;
    }
  }

  void render(Canvas& canvas, bool open) const
  {
    if (open) {
      canvas | Lines{points};
    } else if (outline) {
      canvas | OutlinePoly{points};
    } else {
      canvas | FillPoly{points};
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_POLY_TOOL_INCLUDED */
