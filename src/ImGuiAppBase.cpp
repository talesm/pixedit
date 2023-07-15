#include "ImGuiAppBase.hpp"
#include "imgui.h"
#include "utils/paths.hpp"

namespace pixedit {

ImGuiAppBase::ImGuiAppBase(const SDL_Point& windowSz)
  : window(SDL_CreateWindow("Pixedit viewer",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            windowSz.x,
                            windowSz.y,
                            SDL_WINDOW_RESIZABLE))
  , renderer(SDL_CreateRenderer(window, -1, 0))
  , ui(window, renderer)
  , view{{0, 0, windowSz.x, windowSz.y}}
{
  if (!window || !renderer) { throw std::runtime_error{SDL_GetError()}; }
}

void
ImGuiAppBase::event(const SDL_Event& ev, bool imGuiMayUse)
{
  switch (ev.type) {
  case SDL_QUIT: exited = true; break;
  case SDL_WINDOWEVENT:
    switch (ev.window.event) {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      view.setViewport({0, 0, ev.window.data1, ev.window.data2});
      break;
    default: break;
    }
    break;
  case SDL_MOUSEWHEEL:
    if (ImGui::GetIO().WantCaptureMouse || !showView) break;
    view.state.wheelX += ev.wheel.x;
    view.state.wheelY += ev.wheel.y;
    break;
  case SDL_KEYDOWN: {
    if (ImGui::GetIO().WantCaptureKeyboard) break;
    auto mod = ev.key.keysym.mod;
    shortcuts.exec({.key = ev.key.keysym.sym,
                    .ctrl = (mod & KMOD_CTRL) != 0,
                    .alt = (mod & KMOD_ALT) != 0,
                    .shift = (mod & KMOD_SHIFT) != 0});
    break;
  }
  default: break;
  }
}

int
ImGuiAppBase::run()
{
  while (!exited) {
    // Update
    for (SDL_Event ev; SDL_PollEvent(&ev);) { event(ev, ui.event(ev)); }

    ui.update();
    update();

    if (showView) {
      if (!ImGui::GetIO().WantCaptureMouse && SDL_GetMouseFocus() == window) {
        auto buttonState = SDL_GetMouseState(&view.state.x, &view.state.y);
        view.state.left = buttonState & SDL_BUTTON_LMASK;
        view.state.middle = buttonState & SDL_BUTTON_MMASK;
        view.state.right = buttonState & SDL_BUTTON_RMASK;
      };
      view.update(renderer);
    }

    /// Render
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderClear(renderer);

    if (showView) { view.render(renderer); }

    ui.render();

    SDL_RenderPresent(renderer);
    SDL_Delay(10);
  }
  SDL_Quit();
  return EXIT_SUCCESS;
}

} // namespace pixedit
