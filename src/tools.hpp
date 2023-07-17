#ifndef PIXEDIT_SRC_TOOLS_INCLUDED
#define PIXEDIT_SRC_TOOLS_INCLUDED

#include <vector>
#include "ToolDescription.hpp"

namespace pixedit {

/// @brief Get all registered tools
/// @return
const std::vector<ToolDescription>&
getTools();

const ToolDescription&
getTool(IdRef id);

namespace tools {

constexpr Id MOVE = "Move";
constexpr Id ZOOM = "Zoom";
constexpr Id FREE_HAND = "Free-hand";
constexpr Id LINES = "Lines";
constexpr Id FLOOD_FILL = "Flood fill";
constexpr Id OUTLINE_RECT = "Rect:outline";
constexpr Id FILLED_RECT = "Rect:filled";
constexpr Id OUTLINE_OVAL = "Oval:outline";
constexpr Id FILLED_OVAL = "Oval:filled";
constexpr Id OUTLINE_POLY = "Poly:outline";
constexpr Id FILLED_POLY = "Poly:filled";
constexpr Id RECT_SELECT = "Select:rect";
constexpr Id FREE_HAND_SELECT = "Select:free-hand";
} // namespace tools

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_INCLUDED */
