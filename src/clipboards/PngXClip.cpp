#include "PngXClip.hpp"
#include <cstdlib>
#include "TempSurface.hpp"

namespace pixedit {

bool
copyToXClip(Surface surface)
{
  if (!surface) { return false; }
  TempSurface temp{surface};
  std::string toClipboard =
    "xclip -selection clipboard -t image/png  -in " + temp.getFilename();
  if (std::system(toClipboard.c_str()) != 0) { return false; }
  return true;
}

Surface
copyFromXClip()
{
  TempSurface temp;
  std::string fromClipboard =
    "xclip -selection clipboard -t image/png -out > " + temp.getFilename();
  if (std::system(fromClipboard.c_str()) != 0) { return nullptr; }
  return temp.recover();
}

} // namespace pixedit
