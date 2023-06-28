#include <iostream>
#include <SDL.h>
#include "Buffer.hpp"
#include "Canvas.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

static void
setupImGui();

int
main()
{
  SDL_Init(SDL_INIT_VIDEO);
  constexpr SDL_Point windowSz = {1024, 768};
  window = SDL_CreateWindow("Pixedit viewer",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            windowSz.x,
                            windowSz.y,
                            SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, -1, 0);
  setupImGui();

  using namespace pixedit;
  Canvas canvas{
    .buffer{"../assets/samples/redball_128x128.png"},
    .viewPort{0, 0, windowSz.x, windowSz.y},
  };
  canvas.updatePreview(renderer);

  bool exited = false;
  while (!exited) {
    for (SDL_Event ev; SDL_PollEvent(&ev);) {
      ImGui_ImplSDL2_ProcessEvent(&ev);
      switch (ev.type) {
      case SDL_QUIT: exited = true; break;
      case SDL_WINDOWEVENT:
        switch (ev.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          canvas.viewPort.w = ev.window.data1;
          canvas.viewPort.h = ev.window.data2;
          break;
        default: break;
        }
        break;
      default: break;
      }
    }

    // Update
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    /// Render
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderClear(renderer);

    canvas.render(renderer);

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(renderer);
    SDL_Delay(1);
  }
  return 0;
}

static void
setupImGui()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
    ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
    ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer_Init(renderer);
}
