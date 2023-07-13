#ifndef PIXEDIT_SRC_TOOLS_SELECTION_FREE_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_SELECTION_FREE_TOOL_INCLUDED

#include <vector>
#include "SelectionHandTool.hpp"
#include "primitives/Poly.hpp"
#include "renderSelection.hpp"
#include "utils/cutoutSurface.hpp"

namespace pixedit {

struct SelectionFreeTool
{
  SelectionHandTool handTool;
  bool selecting = false;
  std::vector<SDL_Point> points;
  bool moved = false;
  bool linesMode = false;

  void operator()(PictureView& view, PictureEvent event)
  {
    auto& buffer = *view.getBuffer();
    if (buffer.hasSelection()) {
      handTool(view, event);
      return;
    }
    switch (event) {
    case PictureEvent::LEFT:
      if (selecting) {
        if (!moved) { break; }
        points.emplace_back(view.effectivePos());
      } else {
        selecting = true;
        points.clear();
        points.emplace_back(view.effectivePos());
        renderSelection(view.getGlassCanvas(), points, true);
        linesMode = false;
      }
      moved = true;
      view.previewEdit();
      break;

    case PictureEvent::NONE:
      if (selecting) {
        auto currPoint = view.effectivePos();
        auto& lastPoint = points.back();
        if (currPoint.x == lastPoint.x && currPoint.y == lastPoint.y) {
          renderSelection(view.getGlassCanvas(), points, true);
          break;
        }
        moved = true;
        points.push_back(currPoint);
        renderSelection(view.getGlassCanvas(), points, true);
        if (linesMode) { points.pop_back(); }
        view.previewEdit();
      }
      break;

    case PictureEvent::OK:
      if (selecting) {
        if (moved) {
          moved = false;
          if (!linesMode) {
            if (points.size() == 1) {
              linesMode = true;
            } else {
              renderSelection(view.getGlassCanvas(), points, false);
              finishSelection(buffer);
            }
          }
        } else {
          renderSelection(view.getGlassCanvas(), points, false);
          finishSelection(buffer);
        }
      };
      break;

    case PictureEvent::RIGHT:
      if (selecting) {
        points.emplace_back(view.effectivePos());
        renderSelection(view.getGlassCanvas(), points, false);
        finishSelection(buffer);
      }
      break;

    default:
      selecting = false;
      view.enableGlass(false);
      break;
    }
  }

  void finishSelection(PictureBuffer& buffer)
  {
    SDL_Point p0 = points[0], p1 = points[0];
    for (auto& p : points) {
      p0.x = std::min(p0.x, p.x);
      p0.y = std::min(p0.y, p.y);
      p1.x = std::max(p1.x, p.x);
      p1.y = std::max(p1.y, p.y);
    }
    SDL_Rect rect =
      intersectFromOrigin(Rect::fromPoints(p0, p1), buffer.getSize());
    for (auto& p : points) {
      p.x -= rect.x;
      p.y -= rect.y;
    }
    Surface mask = Surface::createMask(rect.w, rect.h);
    Canvas c{mask};
    c | RawColorA{1} | FillPoly{points};

    buffer.setSelection(
      cutoutSurface(buffer.getSurface(), rect, mask), rect, mask);

    selecting = false;
    points.clear();
  }
};
}

#endif /* PIXEDIT_SRC_TOOLS_SELECTION_FREE_TOOL_INCLUDED */
