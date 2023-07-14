#ifndef PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED

#include <string>
#include <SDL.h>
#include "Clipboard.hpp"
#include "ImGuiAppBase.hpp"
#include "utils/rect.hpp"

namespace pixedit {

/// @brief Startup settings for Editor
struct EditorInitSettings
{
  SDL_Point windowSz;
  std::string filename;
};

struct ViewSettings
{
  SDL_Texture* texture = nullptr;
  SDL_FPoint offset = {0};
  float scale = 1;
  int fileUnamedId = 0;
  std::string filename;
  std::string titleBuffer;
};

class EditorApp : ImGuiAppBase
{
  std::vector<std::shared_ptr<PictureBuffer>> buffers;
  std::map<PictureBuffer*, ViewSettings> viewSettings;
  int bufferIndex = -1;

  Clipboard clipboard;

  std::string requestModal;

public:
  EditorApp(EditorInitSettings settings);
  using ImGuiAppBase::run;

private:
  void setupShortcuts();

  void event(const SDL_Event& ev, bool imGuiMayUse) final;

  void update() final;

  void showNewFileDialog();

  void showPictureWindows()
  {
    for (auto& buffer : buffers) { showPictureWindow(buffer); }
  }

  void showPictureWindow(const std::shared_ptr<PictureBuffer>& buffer);

  void showMainMenuBar();

  void showPictureOptions();

  void appendFile(std::shared_ptr<PictureBuffer> buffer)
  {
    view.setBuffer(buffer);
    view.offset = {0, 0};
    view.scale = 1.f;
    buffers.emplace_back(buffer);
    bufferIndex = buffers.size() - 1;
  }

  /// Actions
  void copy();
  void cut();
  void paste();
  void pasteAsNew();
  void close();

  void focusBufferWindow();
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED */
