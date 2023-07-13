#ifndef PIXEDIT_SRC_UTILS_REPLAY_PICTURE_INCLUDED
#define PIXEDIT_SRC_UTILS_REPLAY_PICTURE_INCLUDED

#include <istream>
#include <string_view>

namespace pixedit {
struct PictureView;
struct Surface;

/// @brief Replay commands in an existing view
/// @param in the command stream
/// @param view the view
void
replayPicture(std::istream& in, PictureView& view, bool allowLoad = false);

/// @brief Replay commands and generate PictureBuffer
/// @param in the command stream
/// @param filename the initial filename, defaults none
Surface
replayPicture(std::istream& in,
              std::string_view filename = {},
              bool allowLoad = false);

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_REPLAY_PICTURE_INCLUDED */
