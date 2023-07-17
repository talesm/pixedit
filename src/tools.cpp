#include "tools.hpp"
#include <unordered_map>
#include "tools/FloodFillTool.hpp"
#include "tools/FreeHandTool.hpp"
#include "tools/LinesTool.hpp"
#include "tools/OvalTool.hpp"
#include "tools/PolyTool.hpp"
#include "tools/RectTool.hpp"
#include "tools/SelectionFreeTool.hpp"
#include "tools/SelectionHandTool.hpp"
#include "tools/SelectionRectTool.hpp"
#include "tools/ZoomTool.hpp"

namespace pixedit {

const std::vector<ToolDescription>&
getTools()
{
  static const std::vector<ToolDescription> tools{
    {"Move", NameId(tools::MOVE), [] { return nullptr; }},
    {"Zoom", NameId(tools::ZOOM), [] { return ZoomTool{}; }},
    {"Pen", NameId(tools::FREE_HAND), [] { return FreeHandTool{}; }},
    {"Lines", NameId(tools::LINES), [] { return LinesTool{}; }},
    {"Flood fill", NameId(tools::FLOOD_FILL), [] { return FloodFillTool{}; }},
    {"Outline rect",
     NameId(tools::OUTLINE_RECT),
     [] { return RectTool{true}; }},
    {"Filled rect", NameId(tools::FILLED_RECT), [] { return RectTool{false}; }},
    {"Outline oval",
     NameId(tools::OUTLINE_OVAL),
     [] { return OvalTool{true}; }},
    {"Filled oval", NameId(tools::FILLED_OVAL), [] { return OvalTool{false}; }},
    {"Outline poly",
     NameId(tools::OUTLINE_POLY),
     [] { return PolyTool{true}; }},
    {"Filled poly", NameId(tools::FILLED_POLY), [] { return PolyTool{false}; }},
    {.name = "Rect select",
     .id = NameId(tools::RECT_SELECT),
     .build = [] { return SelectionRectTool{}; },
     .flags = ToolDescription::ENABLE_SELECTION},
    {.name = "Free form select",
     .id = NameId(tools::FREE_HAND_SELECT),
     .build = [] { return SelectionFreeTool{}; },
     .flags = ToolDescription::ENABLE_SELECTION},
  };
  return tools;
}

const ToolDescription&
getTool(IdRef id)
{
  static auto tools = [] {
    std::unordered_map<IdRef, ToolDescription> tools;
    for (auto& t : getTools()) { tools.emplace(t.id, t); }
    return tools;
  }();
  return tools.at(id);
}

} // namespace pixedit
