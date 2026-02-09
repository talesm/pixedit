#include "Clipboard.hpp"

#include "utils/TempSurface.hpp"

namespace pixedit {

namespace defaults {
extern const int CLIPBOARD_MANAGER;
} // namespace defaults

Surface
Clipboard::get()
{ return SDL::GetClipboardImage(); }

bool
Clipboard::set(const Surface& surface)
{
  TempSurface tempSurface(surface);
  SDL::IOStream stream{SDL::IOFromFile(tempSurface.filename(), "rb")};
  Sint64 size = stream.GetSize();
  auto pointer = new Uint8[size];
  stream.Read(SDL::TargetBytes(pointer, size));
  SDL::SourceBytes{pointer, size_t(size)};

  static const char* mimetypes[] = {"image/png"};
  SDL::SetClipboardData(
    [pointer, size](const char*) {
      return SDL::SourceBytes{pointer, size_t(size)};
    },
    [pointer] { delete[] pointer; },
    mimetypes);
  return true;
}

} // namespace pixedit
