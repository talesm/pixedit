#ifndef PIXEDIT_SRC_CLIPBOARD_INCLUDED
#define PIXEDIT_SRC_CLIPBOARD_INCLUDED

#include "Surface.hpp"

namespace pixedit {

/// @brief Clipboard for images
struct Clipboard
{
  Surface get();

  bool set(Surface surface);
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_CLIPBOARD_INCLUDED */
