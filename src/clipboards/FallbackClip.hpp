#ifndef PIXEDIT_SRC_CLIPBOARDS_FALLBACK_CLIP_INCLUDED
#define PIXEDIT_SRC_CLIPBOARDS_FALLBACK_CLIP_INCLUDED

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
copyToFallback(Surface surface);

/**
 * Copy surface from clipboard
 *
 * @return the surface to be copied
 */
Surface
copyFromFallback();

} // namespace pixedit

#endif /* PIXEDIT_SRC_CLIPBOARDS_FALLBACK_CLIP_INCLUDED */
