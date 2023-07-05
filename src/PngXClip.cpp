#include "PngXClip.hpp"
#include <cstdlib>
#include "TempSurface.hpp"

namespace pixedit {

bool
copyToXClip(SDL_Surface* surface)
{
  if (surface == nullptr) { return false; }
  TempSurface temp{surface};
  std::string toClipboard =
    "xclip -selection clipboard -t image/png  -in " + temp.getFilename();
  if (std::system(toClipboard.c_str()) != 0) { return false; }
  return true;
}
} // namespace pixedit
