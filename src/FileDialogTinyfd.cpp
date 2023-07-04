#include "FileDialogTinyfd.hpp"
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
  auto previousFilename = buffer.filename;
  buffer.filename = filename;
  if (buffer.save()) return true;
  buffer.filename = previousFilename;
  return false;
}

} // namespace pixedit
