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

class EditorApp
{
public:
  EditorApp(EditorInitSettings settings);
  EditorApp(const EditorApp&) = delete;
  ~EditorApp();
  EditorApp& operator=(const EditorApp&) = delete;

  int run();
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED */
