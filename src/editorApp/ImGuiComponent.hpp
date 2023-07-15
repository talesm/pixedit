#ifndef PIXEDIT_SRC_EDITOR_APP_IM_GUI_COMPONENT_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_IM_GUI_COMPONENT_INCLUDED

#include <SDL.h>

namespace pixedit {

struct ImGuiComponent
{
  ImGuiComponent(SDL_Window* window, SDL_Renderer* renderer);
  ~ImGuiComponent();

  bool event(const SDL_Event& ev);
  void update();
  void render();
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_IM_GUI_COMPONENT_INCLUDED */
