#include "Clipboard.hpp"

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
  SDL::IOStreamRef stream{SDL::IOFromDynamicMem()};
  surface.SavePNG(stream);
  Sint64 size = stream.Tell();
  auto pointer = stream.GetProperties().GetPointerProperty(
    SDL::prop::IOStream::DYNAMIC_MEMORY_POINTER, nullptr);
  SDL::SourceBytes{pointer, size_t(size)};

  static const char* mimetypes[] = {"image/png"};
  SDL::SetClipboardData(
    [pointer, size](const char*) {
      return SDL::SourceBytes{pointer, size_t(size)};
    },
    [stream] { SDL::CloseIO(stream.get()); },
    mimetypes);
  return true;
}

} // namespace pixedit
