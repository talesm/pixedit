#ifndef PIXEDIT_SRC_CLIPBOARDS_PNG_XCLIP_INCLUDED
#define PIXEDIT_SRC_CLIPBOARDS_PNG_XCLIP_INCLUDED

#include "Surface.hpp"

namespace pixedit {

/**
 * Copy surface to clipboard
 *
 * @param surface the surface to be copied
 *
 * @return true if copied successfully
 */
bool
copyToXClip(Surface surface);

/**
 * Copy surface from clipboard
 *
 * @return the surface to be copied
 */
Surface
copyFromXClip();

} // namespace pixedit

#endif /* PIXEDIT_SRC_CLIPBOARDS_PNG_XCLIP_INCLUDED */
