#ifndef PIXEDIT_SRC_EDITOR_APP_FILE_DIALOG_TINYFD_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_FILE_DIALOG_TINYFD_INCLUDED

#include <memory>
#include <string>
#include "PictureBuffer.hpp"

namespace pixedit {

std::shared_ptr<PictureBuffer>
loadFromFileDialog(const std::string& initialPath = "");

bool
saveWithFileDialog(PictureBuffer& buffer);

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_FILE_DIALOG_TINYFD_INCLUDED */
