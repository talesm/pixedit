#ifndef PIXEDIT_SRC_TOOLS_SELECTION_FREE_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_SELECTION_FREE_TOOL_INCLUDED

#include <vector>
#include "SelectionHandTool.hpp"
#include "primitives/Poly.hpp"
#include "utils/cutoutSurface.hpp"
#include "utils/renderSelection.hpp"

namespace pixedit {

struct SelectionFreeTool
{
  SelectionHandTool handTool;
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
      if (view.isEditing()) {
        if (!moved) { break; }
        points.emplace_back(view.effectivePos());
      } else {
        view.beginEdit();
        points.clear();
        points.emplace_back(view.effectivePos());
        view.enableScratch();
        renderSelection(view.canvas, points, true);
        linesMode = false;
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
          renderSelection(view.canvas, points, true);
          break;
        }
        moved = true;
        points.push_back(currPoint);
        view.enableScratch();
        renderSelection(view.canvas, points, true);
        if (linesMode) { points.pop_back(); }
        view.previewEdit();
      }
      break;

    case PictureEvent::OK:
      if (view.isEditing()) {
        if (moved) {
          moved = false;
          if (!linesMode) {
            if (points.size() == 1) {
              linesMode = true;
            } else {
              view.cancelEdit();
              auto fillColor = view.canvas.getColorB();
              if (!view.fillSelectedOut) fillColor.a = 0;
              finishSelection(buffer, view.isTransparent(), fillColor);
              handTool(view, PictureEvent::RESET);
            }
          }
        } else {
          view.cancelEdit();
          auto fillColor = view.canvas.getColorB();
          if (!view.fillSelectedOut) fillColor.a = 0;
          finishSelection(buffer, view.isTransparent(), fillColor);
          handTool(view, PictureEvent::RESET);
        }
      };
      break;

    case PictureEvent::RIGHT:
      if (view.isEditing()) {
        view.cancelEdit();
        points.emplace_back(view.effectivePos());
        auto fillColor = view.canvas.getColorB();
        if (!view.fillSelectedOut) fillColor.a = 0;
        finishSelection(buffer, view.isTransparent(), fillColor);
        handTool(view, PictureEvent::RESET);
      }
      break;

    default: view.cancelEdit(); break;
    }
  }

  void finishSelection(PictureBuffer& buffer, bool transparent, Color fillColor)
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

    auto selectionSurface =
      cutoutSurface(buffer.getSurface(), rect, mask, fillColor);
    selectionSurface.setBlendMode(transparent ? SDL_BLENDMODE_BLEND
                                              : SDL_BLENDMODE_NONE);
    buffer.setSelection(selectionSurface, rect, mask);
    points.clear();
  }
};
}

#endif /* PIXEDIT_SRC_TOOLS_SELECTION_FREE_TOOL_INCLUDED */
