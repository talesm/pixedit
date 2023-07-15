#include "defaults.hpp"

namespace pixedit::defaults {

extern const int WINDOW_WIDTH = PIXEDIT_WINDOW_WIDTH;
extern const int WINDOW_HEIGHT = PIXEDIT_WINDOW_HEIGHT;
extern const bool WINDOW_MAXIMIZED = PIXEDIT_WINDOW_MAXIMIZED;
extern const unsigned HISTORY_MAX = PIXEDIT_HISTORY_MAX;

namespace clipboards {
extern const int FALLBACK = CLIPBOARD_FALLBACK;
extern const int XCLIP = CLIPBOARD_XCLIP;
} // namespace clipboard

extern const int CLIPBOARD_MANAGER = PIXEDIT_CLIPBOARD_MANAGER;
extern const bool ASK_SAVE_ON_CLOSE = PIXEDIT_ASK_SAVE_ON_CLOSE;

extern const char INITIAL_FILENAME[] = PIXEDIT_INITIAL_FILENAME;
extern const int INITIAL_SIZE[2] = {PIXEDIT_INITIAL_SIZE};
extern const char ORG_NAME[] = PIXEDIT_ORG_NAME;
extern const char APP_NAME[] = PIXEDIT_APP_NAME;

} // namespace pixedit::defaults
