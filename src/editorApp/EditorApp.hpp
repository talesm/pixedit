#ifndef PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED

#include <string>
#include <SDL.h>
#include "Action.hpp"
#include "Clipboard.hpp"
#include "ImGuiComponent.hpp"
#include "PictureManager.hpp"
#include "PictureView.hpp"
#include "ShortcutManager.hpp"
#include "utils/rect.hpp"

namespace pixedit {

/// @brief Startup settings for Editor
struct EditorInitSettings
{
  SDL_Point windowSz;
  std::string filename;
  SDL_Point pictureSz;
};

struct ViewSettings
{
  PictureView view;
  SDL_Texture* texture = nullptr;
  int fileUnamedId = 0;
  std::string filename;
  std::string titleBuffer;
};

class EditorApp
{
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  ImGuiComponent ui;

  PictureManager picture;
  Rect pictureViewport;
  PictureView view;
  bool exited = false;
  bool maximizeView = false;

  std::vector<std::shared_ptr<PictureBuffer>> buffers;
  std::map<std::shared_ptr<PictureBuffer>, ViewSettings> viewSettings;
  int bufferIndex = -1;

  Clipboard clipboard;
  ActionManager actions;
  ShortcutManager shortcuts;

  bool exiting = false;
  bool focusBufferNextFrame = true;
  std::string requestModal;

  std::shared_ptr<PictureBuffer> currentBuffer()
  {
    if (bufferIndex < 0 || buffers.empty()) return nullptr;
    return buffers[bufferIndex];
  }

  PictureView& currentView()
  {
    if (!maximizeView && currentBuffer()) {
      return viewSettings[currentBuffer()].view;
    }
    return view;
  }

public:
  EditorApp(EditorInitSettings settings);

  int run();

private:
  void setupActions();

  void setupShortcuts();

  void event(const SDL_Event& ev, bool imGuiMayUse);

  void update();

  void showNewFileDialog();

  void showConfirmExitDialog();

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
  void close(bool force = false);
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_EDITOR_APP_INCLUDED */
