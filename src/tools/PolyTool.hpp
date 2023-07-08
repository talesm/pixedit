#ifndef PIXEDIT_SRC_TOOLS_POLY_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_POLY_TOOL_INCLUDED

#include <vector>
#include "PictureTool.hpp"
#include "PictureView.hpp"
#include "primitives/Line.hpp"
#include "rasterPoly.hpp"

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
        view.canvas | LineTo{currPoint, lastPoint};
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
    if (!outline && !open && points.size() >= 3) {
      rasterPoly(
        (const int*)points.data(), points.size(), [&](int x, int y, int len) {
          canvas | HorizontalLine{x, y, len};
        });
      return;
    }
    for (auto it = points.begin() + 1; it != points.end(); ++it) {
      canvas | OpenLineTo(*it, *(it - 1));
    }
    if (open || points.size() < 3) {
      canvas | points.front();
    } else {
      canvas | OpenLineTo(points.front(), points.back());
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_POLY_TOOL_INCLUDED */
