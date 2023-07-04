#include <functional>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <SDL.h>
#include <SDL_image.h>
#include "PictureBuffer.hpp"
#include "PictureView.hpp"
#include "PngXClip.hpp"
#include "ShortcutManager.hpp"
#include "ZoomTool.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include "tinyfiledialogs.h"

namespace pixedit {

struct InitSettings
{
  std::string filename{"../assets/samples/redball_128x128.png"};
  SDL_Point windowSz{1024, 768};
};

struct ToolDescription
{
  const char* name;
  std::function<PictureTool*()> build;
};

ToolDescription tools[2] = {
  {"Pan", [] { return nullptr; }},
  {"Zoom", [] { return new ZoomTool{}; }},
};
class ViewerApp
{
public:
  ViewerApp(InitSettings settings = {});

  int run();

private:
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  PictureView view;
  std::vector<std::shared_ptr<PictureBuffer>> buffers;
  ShortcutManager shortcuts;
  int bufferIndex = -1;
  bool exited = false;
  int toolIndex = 0;

  void handleWindowEvent(const SDL_WindowEvent& ev);
  void handleDropEvent(const SDL_DropEvent& ev);

  void setupShortcuts();
  void setupImGui();

  void showPictureOptions();

  void changeFile(const std::string& filename);
};

ViewerApp::ViewerApp(InitSettings settings)
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
  auto buffer = std::make_shared<PictureBuffer>(std::move(settings.filename));
  view.buffer = buffer;
  buffers.emplace_back(buffer);
  bufferIndex = buffers.size() - 1;
  view.updatePreview(renderer);
  setupImGui();
  setupShortcuts();
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
      case SDL_MOUSEWHEEL:
        if (ImGui::GetIO().WantCaptureMouse) break;
        view.state.wheelX += ev.wheel.x;
        view.state.wheelY += ev.wheel.y;
        break;
      case SDL_DROPFILE:
        if (ImGui::GetIO().WantCaptureMouse) break;
        handleDropEvent(ev.drop);
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
    showPictureOptions();
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

void
ViewerApp::handleWindowEvent(const SDL_WindowEvent& ev)
{
  switch (ev.event) {
  case SDL_WINDOWEVENT_SIZE_CHANGED:
    view.viewPort.w = ev.data1;
    view.viewPort.h = ev.data2;
    break;
  default: break;
  }
}

void
ViewerApp::changeFile(const std::string& filename)
{
  try {
    auto buffer = std::make_shared<PictureBuffer>(filename);
    view.buffer = buffer;
    view.updatePreview(renderer);
    view.offset = {0, 0};
    view.scale = 1.f;
    buffers.emplace_back(buffer);
    bufferIndex = buffers.size() - 1;
  } catch (...) {
    SDL_Log("Failed to open file: %s", filename.c_str());
  }
}

void
ViewerApp::handleDropEvent(const SDL_DropEvent& ev)
{
  changeFile(ev.file);
}

void
ViewerApp::setupShortcuts()
{
  shortcuts.set({.key = SDLK_o, .ctrl = true}, [&] {
    static const char* filePatterns[] = {"*.png", "*.jpg", "*.jpeg", "*.bmp"};
    auto filename =
      tinyfd_openFileDialog("Select file to open",
                            nullptr,
                            sizeof(filePatterns) / sizeof(filePatterns[0]),
                            filePatterns,
                            "Image files",
                            false);
    if (filename) { changeFile(filename); }
  });
  shortcuts.set({.key = SDLK_c, .ctrl = true}, [&] {
    if (view.buffer) copyToXClip(view.buffer->surface);
  });
  auto closeFile = [&] {
    if (buffers.empty()) {
      exited = true;
    } else {
      buffers.erase(buffers.begin() + bufferIndex);
      if (bufferIndex >= int(buffers.size())) { bufferIndex -= 1; }
      if (bufferIndex < 0) {
        view.buffer.reset();
      } else {
        view.buffer = buffers[bufferIndex];
        view.updatePreview(renderer);
      }
    }
  };
  shortcuts.set({.key = SDLK_w, .ctrl = true}, closeFile);
  shortcuts.set({.key = SDLK_F4, .ctrl = true}, closeFile);
  auto funcSaveAs = [&] {
    if (buffers.empty() || bufferIndex < 0) return;
    static const char* filePatterns[] = {"*.png"};
    auto filename =
      tinyfd_saveFileDialog("Save as",
                            view.buffer->filename.c_str(),
                            sizeof(filePatterns) / sizeof(filePatterns[0]),
                            filePatterns,
                            "Image files");
    if (filename) { IMG_SavePNG(view.buffer->surface, filename); }
  };
  shortcuts.set({.key = SDLK_s, .ctrl = true}, funcSaveAs);
  shortcuts.set({.key = SDLK_s, .ctrl = true, .shift = true}, funcSaveAs);
}

void
ViewerApp::showPictureOptions()
{
  if (ImGui::Begin("Picture options")) {
    if (ImGui::BeginCombo(
          "File",
          bufferIndex < 0 ? "None" : buffers[bufferIndex]->filename.c_str())) {
      int i = 0;
      for (auto& b : buffers) {
        bool selected = bufferIndex == i;
        if (ImGui::Selectable(b->filename.c_str(), selected)) {
          bufferIndex = i;
          view.buffer = buffers[bufferIndex];
          view.updatePreview(renderer);
        }
        if (selected) { ImGui::SetItemDefaultFocus(); }
        ++i;
      }
      ImGui::EndCombo();
    }
    int currIndex = toolIndex;
    for (auto& tool : tools) {
      if (ImGui::RadioButton(tool.name, currIndex-- == 0)) {
        delete view.tool;
        view.tool = tool.build();
      }
    }
    ImGui::DragFloat2("offset", &view.offset.x, 1.f, -10000, +10000);
    if (ImGui::Button("Reset offset")) { view.offset = {0}; }
    if (ImGui::ArrowButton("Decrease zoom", ImGuiDir_Left)) { view.scale /= 2; }
    ImGui::SameLine();
    ImGui::Text("%g%%", view.scale * 100);
    ImGui::SameLine();
    if (ImGui::ArrowButton("Increase zoom", ImGuiDir_Right)) {
      view.scale *= 2;
    }
    if (ImGui::CollapsingHeader("Transparency options")) {
      float color1[3] = {
        view.checkerColors[0].r / 255.f,
        view.checkerColors[0].g / 255.f,
        view.checkerColors[0].b / 255.f,
      };
      if (ImGui::ColorEdit3("Color 1", color1)) {
        view.checkerColors[0].r = color1[0] * 255.f;
        view.checkerColors[0].g = color1[1] * 255.f;
        view.checkerColors[0].b = color1[2] * 255.f;
      }
      float color2[3] = {
        view.checkerColors[1].r / 255.f,
        view.checkerColors[1].g / 255.f,
        view.checkerColors[1].b / 255.f,
      };
      if (ImGui::ColorEdit3("Color 2", color2)) {
        view.checkerColors[1].r = color2[0] * 255.f;
        view.checkerColors[1].g = color2[1] * 255.f;
        view.checkerColors[1].b = color2[2] * 255.f;
      }
      ImGui::DragInt("Square size", &view.checkerSize, 1, 1, 1024);
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
