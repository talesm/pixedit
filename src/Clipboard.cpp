#include "Clipboard.hpp"

#include "TempSurface.hpp"

namespace pixedit::Clipboard {

namespace defaults {
extern const int CLIPBOARD_MANAGER;
} // namespace defaults

Surface
get()
{
  return SDL::GetClipboardImage();
}

bool
set(const Surface& surface)
{
  // Avoid pollution
  static std::string filename = makeTempFilename("clip_", ".png");
  TempSurface tempSurface(surface, filename);
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

} // namespace pixedit::Clipboard
