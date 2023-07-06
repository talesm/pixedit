#include "defaults.hpp"

namespace pixedit::defaults {

extern const int WINDOW_WIDTH = PIXEDIT_WINDOW_WIDTH;
extern const int WINDOW_HEIGHT = PIXEDIT_WINDOW_HEIGHT;
extern const bool WINDOW_MAXIMIZED = PIXEDIT_WINDOW_MAXIMIZED;
extern const unsigned HISTORY_MAX = PIXEDIT_HISTORY_MAX;

namespace clipboard {
extern const int FALLBACK = CLIPBOARD_FALLBACK;
extern const int XCLIP = CLIPBOARD_XCLIP;
} // namespace clipboard

extern const int CLIPBOARD_MANAGER = PIXEDIT_CLIPBOARD_MANAGER;
extern const bool ASK_SAVE_ON_CLOSE = PIXEDIT_ASK_SAVE_ON_CLOSE;

} // namespace pixedit::defaults
