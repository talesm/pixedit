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

SDL_Surface*
copyFromXClip()
{
  TempSurface temp;
  std::string fromClipboard =
    "xclip -selection clipboard -t image/png -out | " + temp.getFilename();
  return temp.recover();
}

} // namespace pixedit
