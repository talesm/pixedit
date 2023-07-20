#include "FileDialogTinyfd.hpp"
#include "tinyfiledialogs.h"

namespace pixedit {

std::shared_ptr<PictureBuffer>
loadFromFileDialog(const std::string& initialPath)
{
  static const char* filePatterns[] = {"*.png", "*.jpg", "*.jpeg", "*.bmp"};
  auto filename =
    tinyfd_openFileDialog("Select file to open",
                          initialPath.c_str(),
                          sizeof(filePatterns) / sizeof(filePatterns[0]),
                          filePatterns,
                          "Image files",
                          false);
  if (!filename) { return nullptr; }
  return PictureBuffer::load(filename);
}

bool
saveWithFileDialog(PictureBuffer& buffer)
{
  if (!buffer.getSurface()) { return false; }
  static const char* filePatterns[] = {"*.png"};
  auto filename =
    tinyfd_saveFileDialog("Save as",
                          buffer.getFilename().c_str(),
                          sizeof(filePatterns) / sizeof(filePatterns[0]),
                          filePatterns,
                          "Image files");
  if (!filename) return false;
  return buffer.saveAs(filename);
}

} // namespace pixedit
