#ifndef PIXEDIT_SRC_UTILS_PATHS_INCLUDED
#define PIXEDIT_SRC_UTILS_PATHS_INCLUDED

#include <string>
#include <SDL.h>

namespace pixedit {

/**
 * Base path where app, but might be Read only
 */
const std::string&
getBasePath();

/**
 * Preferred path for storing configuration
 */
const std::string&
getPrefPath();

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_PATHS_INCLUDED */
