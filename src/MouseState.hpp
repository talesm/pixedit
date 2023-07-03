#ifndef PIXEDIT_SRC_MOUSE_STATE_INCLUDED
#define PIXEDIT_SRC_MOUSE_STATE_INCLUDED

namespace pixedit {

/// @brief Mouse state
struct MouseState
{
  int x;
  int y;
  bool left;
  bool middle;
  bool right;
  int wheelX;
  int wheelY;
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_MOUSE_STATE_INCLUDED */
