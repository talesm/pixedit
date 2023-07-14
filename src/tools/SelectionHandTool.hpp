#ifndef PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED

#include <array>
#include "renderSelection.hpp"

namespace pixedit {

struct ScaleAnchor
{
  Rect rect;
  bool scalingHorizontal = false;
  bool scalingVertical = false;
  bool scalingLeft = false;
  bool scalingTop = false;
};

inline std::array<ScaleAnchor, 8>
makeScaleAnchors(const PictureBuffer& buffer, float scale);

struct SelectionHandTool
{
  SDL_Point lastPoint;
  bool moving = false;
  bool scalingHorizontal = false;
  bool scalingVertical = false;
  bool scalingLeft = false;
  bool scalingTop = false;

  void operator()(PictureView& view, PictureEvent event)
  {
    auto& buffer = *view.getBuffer();
    if (!buffer.hasSelection()) {
      view.cancelEdit();
      moving = false;
      return;
    }
    auto& rect = buffer.getSelectionRect();
    auto anchors = makeScaleAnchors(buffer, view.effectiveScale());
    switch (event) {
    case PictureEvent::LEFT:
      view.beginEdit();
      lastPoint = view.effectivePos();
      if (SDL_PointInRect(&lastPoint, &rect)) {
        view.enableScratch();
        for (auto& anchor : anchors) {
          if (SDL_PointInRect(&lastPoint, &anchor.rect)) {
            scalingHorizontal = anchor.scalingHorizontal;
            scalingVertical = anchor.scalingVertical;
            scalingLeft = anchor.scalingLeft;
            scalingTop = anchor.scalingTop;
          }
        }
        if (!scalingHorizontal && !scalingVertical) { moving = true; }
        renderSelection(view.canvas, rect, buffer.getSelectionMask());
        renderAnchors(view.canvas, anchors);
      } else {
        view.persistSelection();
        view.endEdit();
      }
      break;

    case PictureEvent::NONE:
      if (moving) {
        view.enableScratch(false);
        auto currPoint = view.effectivePos();
        if (currPoint.x == lastPoint.x && currPoint.y == lastPoint.y) break;
        rect.x += currPoint.x - lastPoint.x;
        rect.y += currPoint.y - lastPoint.y;
        lastPoint = currPoint;
        view.previewEdit();
      } else {
        auto currPoint = view.effectivePos();
        if (scalingHorizontal) {
          if (scalingLeft) {
            rect.x += currPoint.x - lastPoint.x;
            rect.w -= currPoint.x - lastPoint.x;
          } else {
            rect.w += currPoint.x - lastPoint.x;
          }
        }
        if (scalingVertical) {
          if (scalingTop) {
            rect.y += currPoint.y - lastPoint.y;
            rect.h -= currPoint.y - lastPoint.y;
          } else {
            rect.h += currPoint.y - lastPoint.y;
          }
        }
        lastPoint = currPoint;
        view.enableScratch();
        renderSelection(view.canvas, rect, buffer.getSelectionMask());
        renderAnchors(view.canvas, anchors);
      }
      break;

    case PictureEvent::OK:
      if (moving) {
        view.enableScratch();
        renderSelection(view.canvas, rect, buffer.getSelectionMask());
        renderAnchors(view.canvas, anchors);
        moving = false;
      } else if (scalingHorizontal || scalingVertical) {
        scalingHorizontal = scalingVertical = false;
        scalingTop = scalingLeft = false;
      }
      break;
    case PictureEvent::RESET: break;

    default:
      view.persistSelection();
      view.endEdit();
      scalingHorizontal = scalingVertical = false;
      scalingTop = scalingLeft = false;
      break;
    }
  }

  void renderAnchors(Canvas& canvas, const std::array<ScaleAnchor, 8>& anchors)
  {
    auto bkp = canvas.getBrush();
    canvas | Pen{} | patterns::SOLID | ColorA{127, 255, 255, 200};

    for (auto& anchor : anchors) {
      if (!anchor.rect.w) { continue; }
      canvas | FillRect(anchor.rect);
    }
    canvas | bkp;
  }
};

inline std::array<ScaleAnchor, 8>
makeScaleAnchors(const PictureBuffer& buffer, float scale)
{
  Rect rect = buffer.getSelectionRect();
  int smallerSize = std::min(rect.w, rect.h);
  if (smallerSize < 2 || smallerSize * scale < 2) { return {}; }
  if (smallerSize < 5 || smallerSize * scale < 5) {
    int sz = ceil(1 / scale);
    return {ScaleAnchor{
      .rect = {rect.x + rect.w - sz, rect.y + rect.h - sz, sz, sz},
      .scalingHorizontal = true,
      .scalingVertical = true,
    }};
  }
  if (smallerSize < 10 || smallerSize * scale < 10) {
    int sz = ceil(2 / scale);
    return {
      ScaleAnchor{
        .rect = {rect.x, rect.y, sz, sz},
        .scalingHorizontal = true,
        .scalingVertical = true,
        .scalingLeft = true,
        .scalingTop = true,
      },
      ScaleAnchor{
        .rect = {rect.x + rect.w - sz, rect.y, sz, sz},
        .scalingHorizontal = true,
        .scalingVertical = true,
        .scalingTop = true,
      },
      ScaleAnchor{
        .rect = {rect.x, rect.y + rect.h - sz, sz, sz},
        .scalingHorizontal = true,
        .scalingVertical = true,
        .scalingLeft = true,
      },
      ScaleAnchor{
        .rect = {rect.x + rect.w - sz, rect.y + rect.h - sz, sz, sz},
        .scalingHorizontal = true,
        .scalingVertical = true,
      },
    };
  }
  float maxVal = scale > 1 ? 16 : std::min(16 / scale, smallerSize / 3.f - 1);
  int sz = std::clamp((smallerSize / scale) / 4, 2.f, maxVal);
  return {
    ScaleAnchor{
      .rect = {rect.x, rect.y, sz, sz},
      .scalingHorizontal = true,
      .scalingVertical = true,
      .scalingLeft = true,
      .scalingTop = true,
    },
    ScaleAnchor{
      .rect = {rect.x + (rect.w - sz) / 2, rect.y, sz, sz},
      .scalingVertical = true,
      .scalingTop = true,
    },
    ScaleAnchor{
      .rect = {rect.x + rect.w - sz, rect.y, sz, sz},
      .scalingHorizontal = true,
      .scalingVertical = true,
      .scalingTop = true,
    },
    ScaleAnchor{
      .rect = {rect.x + rect.w - sz, rect.y + (rect.h - sz) / 2, sz, sz},
      .scalingHorizontal = true,
    },
    ScaleAnchor{
      .rect = {rect.x + rect.w - sz, rect.y + rect.h - sz, sz, sz},
      .scalingHorizontal = true,
      .scalingVertical = true,
    },
    ScaleAnchor{
      .rect = {rect.x + (rect.w - sz) / 2, rect.y + rect.h - sz, sz, sz},
      .scalingVertical = true,
    },
    ScaleAnchor{
      .rect = {rect.x, rect.y + rect.h - sz, sz, sz},
      .scalingHorizontal = true,
      .scalingVertical = true,
      .scalingLeft = true,
    },
    ScaleAnchor{
      .rect = {rect.x, rect.y + (rect.h - sz) / 2, sz, sz},
      .scalingHorizontal = true,
      .scalingLeft = true,
    },
  };
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_SELECTION_HAND_TOOL_INCLUDED */
