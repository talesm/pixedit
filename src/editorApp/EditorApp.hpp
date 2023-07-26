#ifndef PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED

#include <string>
#include "utils/rect.hpp"

namespace pixedit {

/// @brief Startup settings for Editor
struct EditorInitSettings
{
  Point windowSz;
  std::string filename;
  Point pictureSz;
};

int
runEditorApp(EditorInitSettings settings);

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED */
