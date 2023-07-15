/// @file defaults.hpp
/// This defines the default configuration, set at compile time
#ifndef PIXEDIT_SRC_DEFAULTS_INCLUDED
#define PIXEDIT_SRC_DEFAULTS_INCLUDED

#if __has_include("pixedit_config.h")
#include "pixedit_config.h"
#endif // __has_include("pixedit_config.h")

#ifndef PIXEDIT_WINDOW_WIDTH
#define PIXEDIT_WINDOW_WIDTH 1024
#endif // PIXEDIT_WINDOW_WIDTH

#ifndef PIXEDIT_WINDOW_HEIGHT
#define PIXEDIT_WINDOW_HEIGHT 768
#endif // PIXEDIT_WINDOW_HEIGHT

#ifndef PIXEDIT_WINDOW_MAXIMIZED
#define PIXEDIT_WINDOW_MAXIMIZED 0
#endif // PIXEDIT_WINDOW_MAXIMIZED

// The max history size
#ifndef PIXEDIT_HISTORY_MAX
#define PIXEDIT_HISTORY_MAX 30
#endif // PIXEDIT_HISTORY_MAX

#define CLIPBOARD_FALLBACK 0
#define CLIPBOARD_XCLIP 1

// How manage the clipboard? See CLIPBOARD_* for options
#ifndef PIXEDIT_CLIPBOARD_MANAGER
#define PIXEDIT_CLIPBOARD_MANAGER CLIPBOARD_XCLIP
#endif // PIXEDIT_CLIPBOARD_MANAGER

// If we need to ask for saving before closing a modified file
#ifndef PIXEDIT_ASK_SAVE_ON_CLOSE
#define PIXEDIT_ASK_SAVE_ON_CLOSE 1
#endif // PIXEDIT_ASK_SAVE_ON_CLOSE

// The default file to load (Use "" for none)
#ifndef PIXEDIT_INITIAL_FILENAME
#define PIXEDIT_INITIAL_FILENAME ""
#endif // PIXEDIT_INITIAL_FILENAME

// The blank file size (only used if PIXEDIT_INITIAL_FILENAME is "")
#ifndef PIXEDIT_INITIAL_SIZE
#define PIXEDIT_INITIAL_SIZE 128, 128
#endif // PIXEDIT_INITIAL_SIZE

// Default org name for configuring user directory
#ifndef PIXEDIT_ORG_NAME
#define PIXEDIT_ORG_NAME "jungleOwl2"
#endif // PIXEDIT_ORG_NAME

// Default program name for configuring user directory
#ifndef PIXEDIT_APP_NAME
#define PIXEDIT_APP_NAME "Pixedit"
#endif // PIXEDIT_APP_NAME

#endif /* PIXEDIT_SRC_DEFAULTS_INCLUDED */
