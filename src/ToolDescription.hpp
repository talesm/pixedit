#ifndef PIXEDIT_SRC_TOOL_DESCRIPTION_INCLUDED
#define PIXEDIT_SRC_TOOL_DESCRIPTION_INCLUDED

#include <functional>
#include <string>
#include <SDL.h>

namespace pixedit {

// Forward decl
struct PictureView;

/// @brief Represents the event ocurrent on this frame
enum class PictureEvent
{
  NONE,     ///< @brief Just idle
  OK,       ///< @brief Last clicked mouse button released
  CANCEL,   ///< @brief a second button was clicked
  LEFT,     ///< @brief Left button down
  RIGHT,    ///< @brief Right button up
  RESET,    ///< @brief When the state must be set back to initial
  DETACHED, ///< @brief When the state must be set back to initial
};

/// @brief Represents an unique id for a tool
using ToolId = int;

/// @brief Represents a tool instance
using Tool = std::function<void(PictureView& view, PictureEvent event)>;

struct ToolDescription
{
  std::string name;
  ToolId id;
  std::function<Tool()> build;
  Uint32 flags = 0;

  static constexpr Uint32 ENABLE_SELECTION = 1;
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOL_DESCRIPTION_INCLUDED */
