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

struct InitSettings
{
  std::string filename{"../assets/samples/redball_128x128.png"};
  SDL_Point windowSz{1024, 768};
};

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
  std::vector<std::shared_ptr<PictureBuffer>> buffers;
  int bufferIndex = -1;
  bool exited = false;

  ImGuiAppBase(const InitSettings& settings);

  int run();

  void setupImGui();
  virtual void showPictureOptions();

  void appendFile(std::shared_ptr<PictureBuffer> buffer);
};
} // namespace pixedit

#endif /* PIXEDIT_SRC_IM_GUI_APP_BASE_INCLUDED */
