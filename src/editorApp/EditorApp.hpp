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

struct EditorApp
{
protected:
  EditorApp() = default;

public:
  virtual ~EditorApp() = default;

  virtual SDL::AppResult Iterate() = 0;

  virtual SDL::AppResult Event(const SDL::Event& e) = 0;
};

std::unique_ptr<EditorApp>
createEditorApp(const EditorInitSettings& settings);

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED */
