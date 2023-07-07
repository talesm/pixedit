#ifndef PIXEDIT_SRC_TOOLS_PICTURE_TOOL_INCLUDED
#define PIXEDIT_SRC_TOOLS_PICTURE_TOOL_INCLUDED

#include <SDL.h>

namespace pixedit {

// Forward decl
struct PictureView;

/// @brief Represents the event ocurrent on this frame
enum class PictureEvent
{
  NONE,   ///< @brief Just idle
  OK,     ///< @brief Last clicked mouse button released
  CANCEL, ///< @brief a second button was clicked
  LEFT,   ///< @brief Left button down
  RIGHT,  ///< @brief Right button up
};

struct PictureTool
{
  virtual ~PictureTool() = default;

  virtual void update(PictureView& view, PictureEvent event) = 0;

  virtual bool acceptsSelection() const { return false; }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_TOOLS_PICTURE_TOOL_INCLUDED */
