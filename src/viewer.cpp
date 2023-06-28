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

static void
showCanvasOptions(pixedit::Canvas& canvas);

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
      case SDL_MOUSEMOTION:
        if (ImGui::GetIO().WantCaptureMouse ||
            !(ev.motion.state & SDL_BUTTON_LMASK)) {
          break;
        }
        canvas.offset.x += ev.motion.xrel;
        canvas.offset.y += ev.motion.yrel;
        break;
      case SDL_MOUSEWHEEL:
        if (ImGui::GetIO().WantCaptureMouse) { break; }
        if (ev.wheel.y < 0) {
          canvas.scale /= 2;
        } else if (ev.wheel.y > 0) {
          canvas.scale *= 2;
        }
        break;
      default: break;
      }
    }

    // Update
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    showCanvasOptions(canvas);

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

static void
showCanvasOptions(pixedit::Canvas& canvas)
{
  if (ImGui::Begin("Canvas options")) {
    ImGui::DragFloat2("offset", &canvas.offset.x, 1.f, -10000, +10000);
    if (ImGui::Button("Reset offset")) { canvas.offset = {0}; }
    if (ImGui::ArrowButton("Decrease zoom", ImGuiDir_Left)) {
      canvas.scale /= 2;
    }
    ImGui::SameLine();
    ImGui::Text("%g%%", canvas.scale * 100);
    ImGui::SameLine();
    if (ImGui::ArrowButton("Increase zoom", ImGuiDir_Right)) {
      canvas.scale *= 2;
    }
    if (ImGui::CollapsingHeader("Transparency options")) {
      float color1[3] = {
        canvas.checkerColors[0].r / 255.f,
        canvas.checkerColors[0].g / 255.f,
        canvas.checkerColors[0].b / 255.f,
      };
      if (ImGui::ColorEdit3("Color 1", color1)) {
        canvas.checkerColors[0].r = color1[0] * 255.f;
        canvas.checkerColors[0].g = color1[1] * 255.f;
        canvas.checkerColors[0].b = color1[2] * 255.f;
      }
      float color2[3] = {
        canvas.checkerColors[1].r / 255.f,
        canvas.checkerColors[1].g / 255.f,
        canvas.checkerColors[1].b / 255.f,
      };
      if (ImGui::ColorEdit3("Color 2", color2)) {
        canvas.checkerColors[1].r = color2[0] * 255.f;
        canvas.checkerColors[1].g = color2[1] * 255.f;
        canvas.checkerColors[1].b = color2[2] * 255.f;
      }
      ImGui::DragInt("Square size", &canvas.checkerSize, 1, 1, 1024);
    }
  }
  ImGui::End();
}
