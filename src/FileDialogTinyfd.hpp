#ifndef PIXEDIT_SRC_FILE_DIALOG_TINYFD_INCLUDED
#define PIXEDIT_SRC_FILE_DIALOG_TINYFD_INCLUDED

#include <memory>
#include <string>
#include "PictureBuffer.hpp"

namespace pixedit {

std::shared_ptr<PictureBuffer>
loadFromFileDialog(const std::string& initialPath);

bool
saveWithFileDialog(const std::string& initialPath, PictureBuffer& buffer);

} // namespace pixedit

#endif /* PIXEDIT_SRC_FILE_DIALOG_TINYFD_INCLUDED */
