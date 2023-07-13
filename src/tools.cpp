#include "tools.hpp"
#include "tools/FloodFillTool.hpp"
#include "tools/FreeHandTool.hpp"
#include "tools/LinesTool.hpp"
#include "tools/OvalTool.hpp"
#include "tools/PolyTool.hpp"
#include "tools/RectTool.hpp"
#include "tools/SelectionHandTool.hpp"
#include "tools/SelectionRectTool.hpp"
#include "tools/ZoomTool.hpp"

namespace pixedit {

const std::vector<ToolDescription>&
getTools()
{
  static const std::vector<ToolDescription> tools{
    {"Move", tools::MOVE, [] { return nullptr; }},
    {"Zoom", tools::ZOOM, [] { return ZoomTool{}; }},
    {"Free hand", tools::FREE_HAND, [] { return FreeHandTool{}; }},
    {"Lines", tools::LINES, [] { return LinesTool{}; }},
    {"Flood fill", tools::FLOOD_FILL, [] { return FloodFillTool{}; }},
    {"Outline rect", tools::OUTLINE_RECT, [] { return RectTool{true}; }},
    {"Filled rect", tools::FILLED_RECT, [] { return RectTool{false}; }},
    {"Outline oval", tools::OUTLINE_OVAL, [] { return OvalTool{true}; }},
    {"Filled oval", tools::FILLED_OVAL, [] { return OvalTool{false}; }},
    {"Outline poly", tools::OUTLINE_POLY, [] { return PolyTool{true}; }},
    {"Filled poly", tools::FILLED_POLY, [] { return PolyTool{false}; }},
    {.name = "Rect select",
     .id = tools::RECT_SELECT,
     .build = [] { return SelectionRectTool{}; },
     .flags = ToolDescription::ENABLE_SELECTION},
  };
  return tools;
}
} // namespace pixedit
