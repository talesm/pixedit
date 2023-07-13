#ifndef PIXEDIT_SRC_TOOLS_INCLUDED
#define PIXEDIT_SRC_TOOLS_INCLUDED

#include <vector>
#include "ToolDescription.hpp"

namespace pixedit {

/// @brief Get all registered tools
/// @return
const std::vector<ToolDescription>&
getTools();

inline const ToolDescription&
getTool(ToolId id)
{
  // FIXME this is not safe
  return getTools().at(id);
}

namespace tools {

constexpr ToolId MOVE = 0;
constexpr ToolId ZOOM = 1;
constexpr ToolId FREE_HAND = 2;
constexpr ToolId LINES = 3;
constexpr ToolId FLOOD_FILL = 4;
constexpr ToolId OUTLINE_RECT = 5;
constexpr ToolId FILLED_RECT = 6;
constexpr ToolId OUTLINE_OVAL = 7;
constexpr ToolId FILLED_OVAL = 8;
constexpr ToolId OUTLINE_POLY = 9;
constexpr ToolId FILLED_POLY = 10;
constexpr ToolId RECT_SELECT = 11;
constexpr ToolId FREE_HAND_SELECT = 12;
} // namespace tools

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_INCLUDED */
