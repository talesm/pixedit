#ifndef PIXEDIT_SRC_IM_GUI_APP_BASE_INCLUDED
#define PIXEDIT_SRC_IM_GUI_APP_BASE_INCLUDED

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <SDL.h>
#include "PictureBuffer.hpp"
#include "PictureTool.hpp"
#include "PictureView.hpp"
#include "ShortcutManager.hpp"

namespace pixedit {

struct ToolDescription
{
  std::string name;
  std::function<PictureTool*()> build;
};

struct ImGuiAppBase
{
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  std::vector<ToolDescription> tools;
  int toolIndex = 0;
  ShortcutManager shortcuts;
  PictureView view;
  bool exited = false;

  ImGuiAppBase(const SDL_Point& windowSz);

  int run();

  virtual void event(const SDL_Event& ev, bool imGuiMayUse);

  virtual void update() {}

  void setupImGui();
};
} // namespace pixedit

#endif /* PIXEDIT_SRC_IM_GUI_APP_BASE_INCLUDED */
