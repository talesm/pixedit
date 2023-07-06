#include "ImGuiAppBase.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"

namespace pixedit {

ImGuiAppBase::ImGuiAppBase(const InitSettings& settings)
  : window(SDL_CreateWindow("Pixedit viewer",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            settings.windowSz.x,
                            settings.windowSz.y,
                            SDL_WINDOW_RESIZABLE))
  , renderer(SDL_CreateRenderer(window, -1, 0))
  , view{{0, 0, settings.windowSz.x, settings.windowSz.y}}
{
  if (!window || !renderer) { throw std::runtime_error{SDL_GetError()}; }
  setupImGui();
}

int
ImGuiAppBase::run()
{
  while (!exited) {
    // Events
    for (SDL_Event ev; SDL_PollEvent(&ev);) {
      ImGui_ImplSDL2_ProcessEvent(&ev);
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
        if (ImGui::GetIO().WantCaptureMouse) break;
        view.state.wheelX += ev.wheel.x;
        view.state.wheelY += ev.wheel.y;
        break;
      case SDL_DROPFILE:
        if (ImGui::GetIO().WantCaptureMouse) break;
        // appendFile(std::make_shared<PictureBuffer>(ev.drop.file));
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

    // Update
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    update();
    ImGui::ShowDemoWindow();

    if (!ImGui::GetIO().WantCaptureMouse && SDL_GetMouseFocus() == window) {
      auto buttonState = SDL_GetMouseState(&view.state.x, &view.state.y);
      view.state.left = buttonState & SDL_BUTTON_LMASK;
      view.state.middle = buttonState & SDL_BUTTON_MMASK;
      view.state.right = buttonState & SDL_BUTTON_RMASK;
    };
    view.update(renderer);

    /// Render
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderClear(renderer);

    view.render(renderer);

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(renderer);
    SDL_Delay(10);
  }
  ImGui::DestroyContext(nullptr);
  SDL_Quit();
  return EXIT_SUCCESS;
}

void
ImGuiAppBase::setupImGui()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  // io.ConfigFlags |=
  //   ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
    ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer_Init(renderer);
}

} // namespace pixedit
