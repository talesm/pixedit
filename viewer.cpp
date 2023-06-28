#include <iostream>
#include <stdexcept>
#include <SDL.h>
#include "Buffer.hpp"
#include "Canvas.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"

namespace pixedit {

struct InitSettings
{
  std::string filename{"../assets/samples/redball_128x128.png"};
  SDL_Point windowSz{1024, 768};
};

class ViewerApp
{
public:
  ViewerApp(InitSettings settings = {});

  int run();

private:
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  Canvas canvas;
  bool exited = false;

  void handleWindowEvent(const SDL_WindowEvent& ev);
  void handleMotionEvent(const SDL_MouseMotionEvent& ev);
  void handleWheelEvent(const SDL_MouseWheelEvent& ev);
  void handleDropEvent(const SDL_DropEvent& ev);

  void setupImGui();

  void showCanvasOptions();
};

ViewerApp::ViewerApp(InitSettings settings)
  : window(SDL_CreateWindow("Pixedit viewer",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            settings.windowSz.x,
                            settings.windowSz.y,
                            SDL_WINDOW_RESIZABLE))
  , renderer(SDL_CreateRenderer(window, -1, 0))
  , canvas{
      .buffer{std::move(settings.filename)},
      .viewPort{0, 0, settings.windowSz.x, settings.windowSz.y},
    }
{
  if (!window || !renderer) { throw std::runtime_error{SDL_GetError()}; }
  canvas.updatePreview(renderer);
  setupImGui();
}

int
ViewerApp::run()
{
  while (!exited) {
    // Events
    for (SDL_Event ev; SDL_PollEvent(&ev);) {
      ImGui_ImplSDL2_ProcessEvent(&ev);
      switch (ev.type) {
      case SDL_QUIT: exited = true; break;
      case SDL_WINDOWEVENT: handleWindowEvent(ev.window); break;
      case SDL_MOUSEMOTION:
        if (ImGui::GetIO().WantCaptureMouse) break;
        handleMotionEvent(ev.motion);
        break;
      case SDL_MOUSEWHEEL:
        if (ImGui::GetIO().WantCaptureMouse) break;
        handleWheelEvent(ev.wheel);
        break;
      case SDL_DROPFILE:
        if (ImGui::GetIO().WantCaptureMouse) break;
        handleDropEvent(ev.drop);
        break;
      default: break;
      }
    }

    // Update
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    showCanvasOptions();
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
  ImGui::DestroyContext(nullptr);
  SDL_Quit();
  return EXIT_SUCCESS;
}

void
ViewerApp::setupImGui()
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

void
ViewerApp::handleWindowEvent(const SDL_WindowEvent& ev)
{
  switch (ev.event) {
  case SDL_WINDOWEVENT_SIZE_CHANGED:
    canvas.viewPort.w = ev.data1;
    canvas.viewPort.h = ev.data2;
    break;
  default: break;
  }
}

void
ViewerApp::handleMotionEvent(const SDL_MouseMotionEvent& ev)
{
  if (!(ev.state & SDL_BUTTON_LMASK)) { return; }
  canvas.offset.x += ev.xrel;
  canvas.offset.y += ev.yrel;
}

void
ViewerApp::handleWheelEvent(const SDL_MouseWheelEvent& ev)
{
  if (ev.y < 0) {
    canvas.scale /= 2;
  } else if (ev.y > 0) {
    canvas.scale *= 2;
  }
}

void
ViewerApp::handleDropEvent(const SDL_DropEvent& ev)
{
  try {
    canvas.buffer = Buffer{ev.file};
  } catch (...) {
    SDL_Log("Failed to open file: %s", ev.file);
  }
  canvas.updatePreview(renderer);
  canvas.offset = {0, 0};
  canvas.scale = 1.f;
}

void
ViewerApp::showCanvasOptions()
{
  if (ImGui::Begin("Canvas options")) {
    auto& filename = canvas.buffer.filename;
    size_t pos = filename.find_last_of('/') + 1;
    if (pos >= std::string::npos) pos = 0;
    ImGui::InputText(
      "File", filename.data() + pos, filename.size() - pos + 1, 0);
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
}

int
main(int argc, char** argv)
{
  try {
    if (SDL_Init(SDL_INIT_VIDEO)) throw std::runtime_error{SDL_GetError()};
    pixedit::InitSettings settings;
    if (argc > 1) { settings.filename = argv[argc - 1]; }

    pixedit::ViewerApp app{settings};
    return app.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
    std::cerr << "Unknown error\n";
  }
  return EXIT_FAILURE;
}
