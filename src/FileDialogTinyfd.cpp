#include "FileDialogTinyfd.hpp"
#include <SDL_image.h>
#include "tinyfiledialogs.h"

namespace pixedit {

std::shared_ptr<PictureBuffer>
loadFromFileDialog(const std::string& initialPath)
{
  static const char* filePatterns[] = {"*.png", "*.jpg", "*.jpeg", "*.bmp"};
  auto filename =
    tinyfd_openFileDialog("Select file to open",
                          nullptr,
                          sizeof(filePatterns) / sizeof(filePatterns[0]),
                          filePatterns,
                          "Image files",
                          false);
  if (!filename) { return nullptr; }
  return std::make_shared<PictureBuffer>(filename);
}

bool
saveWithFileDialog(const std::string& initialPath, PictureBuffer& buffer)
{
  if (!buffer.surface) { return false; }
  static const char* filePatterns[] = {"*.png"};
  auto filename =
    tinyfd_saveFileDialog("Save as",
                          buffer.filename.c_str(),
                          sizeof(filePatterns) / sizeof(filePatterns[0]),
                          filePatterns,
                          "Image files");
  if (!filename) return false;
  if (IMG_SavePNG(buffer.surface, filename) < 0) return false;
  buffer.filename = filename;
  return true;
}

} // namespace pixedit
