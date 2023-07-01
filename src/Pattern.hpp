#ifndef PIXEDIT_SRC_PATTERN_INCLUDED
#define PIXEDIT_SRC_PATTERN_INCLUDED

#include <SDL.h>

namespace pixedit {

/// @brief The brush filling
struct Pattern
{
  Uint64 data8x8;
};

namespace patterns {
constexpr Pattern SOLID{0};
constexpr Pattern CHECKERED{0x55AA55AA'55AA55AA};
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_PATTERN_INCLUDED */
