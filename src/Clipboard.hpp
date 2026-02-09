#ifndef PIXEDIT_SRC_CLIPBOARD_INCLUDED
#define PIXEDIT_SRC_CLIPBOARD_INCLUDED

#include "Surface.hpp"

namespace pixedit::Clipboard {

Surface
get();

bool
set(const Surface& surface);

} // namespace pixedit::Clipboard

#endif /* PIXEDIT_SRC_CLIPBOARD_INCLUDED */
