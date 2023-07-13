#include "FallbackClip.hpp"

namespace pixedit {

static const char* tempClip = "fallback_clipboard.png";

bool
copyToFallback(Surface surface)
{
  if (!surface) { return false; }
  return surface.save(tempClip);
}

Surface
copyFromFallback()
{
  return Surface::load(tempClip);
}

} // namespace pixedit
