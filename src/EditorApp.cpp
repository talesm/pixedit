#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include "Clipboard.hpp"
#include "FileDialogTinyfd.hpp"
#include "ImGuiAppBase.hpp"
#include "tools/FreeHandTool.hpp"
#include "tools/LinesTool.hpp"
#include "tools/RectTool.hpp"
#include "tools/SelectionHandTool.hpp"
#include "tools/ZoomTool.hpp"

namespace pixedit {

namespace defaults {
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const bool WINDOW_MAXIMIZED;
} // namespace defaults

struct EditorInitSettings
{
  SDL_Point windowSz;
  std::string filename;
};
class EditorApp : ImGuiAppBase
{
  std::vector<std::shared_ptr<PictureBuffer>> buffers;
  int bufferIndex = -1;

  Clipboard clipboard;

public:
  EditorApp(EditorInitSettings settings);
  using ImGuiAppBase::run;

private:
  void setupShortcuts();

  void event(const SDL_Event& ev, bool imGuiMayUse) final;

  void update() final
  {
    if (ImGui::Begin("Picture options")) { showPictureOptions(); }
    ImGui::End();
  }

  void showPictureOptions()
  {
    if (ImGui::BeginCombo("File",
                          bufferIndex < 0
                            ? "None"
                            : buffers[bufferIndex]->getFilename().c_str())) {
      int i = 0;
      for (auto& b : buffers) {
        bool selected = bufferIndex == i;
        if (ImGui::Selectable(b->getFilename().c_str(), selected)) {
          bufferIndex = i;
          view.setBuffer(buffers[bufferIndex]);
        }
        if (selected) { ImGui::SetItemDefaultFocus(); }
        ++i;
      }
      ImGui::EndCombo();
    }

    if (ImGui::CollapsingHeader("Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
      int i = 0;
      for (auto& tool : tools) {
        if (ImGui::RadioButton(tool.name.c_str(), i == toolIndex)) {
          delete view.tool;
          view.tool = tool.build();
          toolIndex = i;
        }
        ++i;
      }
    }
    ImGui::DragFloat2("##offset", &view.offset.x, 1.f, -10000, +10000);
    ImGui::SameLine();
    if (ImGui::Button("Reset##offset")) { view.offset = {0}; }
    if (ImGui::ArrowButton("Decrease zoom", ImGuiDir_Left)) { view.scale /= 2; }
    ImGui::SameLine();
    ImGui::Text("%g%%", view.scale * 100);
    ImGui::SameLine();
    if (ImGui::ArrowButton("Increase zoom", ImGuiDir_Right)) {
      view.scale *= 2;
    }

    if (ImGui::Button(
          "Swap colors",
          {0, ImGui::GetFrameHeightWithSpacing() + ImGui::GetFrameHeight()})) {
      view.swapColors();
    }
    ImGui::SameLine();
    {
      ImGui::BeginChild("Colors");
      auto colorA = componentToNormalized(view.canvas.getColorA());
      if (ImGui::ColorEdit4(
            "Color A", colorA.data(), ImGuiColorEditFlags_NoInputs)) {
        view.canvas | ColorA{normalizedToComponent(colorA)};
      }
      auto colorB = componentToNormalized(view.canvas.getColorB());
      if (ImGui::ColorEdit4(
            "Color B", colorB.data(), ImGuiColorEditFlags_NoInputs)) {
        view.canvas | ColorB{normalizedToComponent(colorB)};
      }
      ImGui::EndChild();
    }
  }

  void appendFile(std::shared_ptr<PictureBuffer> buffer)
  {
    view.setBuffer(buffer);
    view.offset = {0, 0};
    view.scale = 1.f;
    buffers.emplace_back(buffer);
    bufferIndex = buffers.size() - 1;
  }
};

EditorApp::EditorApp(EditorInitSettings settings)
  : ImGuiAppBase(settings.windowSz)
{
  auto buffer = std::make_shared<PictureBuffer>(settings.filename);
  view.setBuffer(buffer);
  buffers.emplace_back(buffer);
  bufferIndex = buffers.size() - 1;

  setupShortcuts();

  tools.emplace_back("Pan", [] { return nullptr; });
  tools.emplace_back("Zoom", [] { return new ZoomTool{}; });
  tools.emplace_back("Free hand", [] { return new FreeHandTool{}; });
  tools.emplace_back("Lines", [] { return new LinesTool{}; });
  tools.emplace_back("Outline rect", [] { return new RectTool{true}; });
  tools.emplace_back("Filled rect", [] { return new RectTool{false}; });
  tools.emplace_back("Rect select", [] { return new SelectionHandTool{}; });

  view.canvas | ColorA{0, 0, 0, 255};
  view.canvas | ColorB{255, 255, 255, 255};
}

void
EditorApp::event(const SDL_Event& ev, bool imGuiMayUse)
{
  switch (ev.type) {
  case SDL_DROPFILE:
    if (ImGui::GetIO().WantCaptureMouse) break;
    appendFile(std::make_shared<PictureBuffer>(ev.drop.file));
    break;

  default: break;
  }
  return ImGuiAppBase::event(ev, imGuiMayUse);
}

void
EditorApp::setupShortcuts()
{
  shortcuts.set({.key = SDLK_o, .ctrl = true}, [&] {
    auto buffer = loadFromFileDialog("./");
    if (buffer) { appendFile(buffer); };
  });
  shortcuts.set({.key = SDLK_c, .ctrl = true}, [&] {
    if (view.getBuffer()) clipboard.set(view.getBuffer()->getSurface());
  });
  auto closeFile = [&] {
    if (buffers.empty()) {
      exited = true;
    } else {
      buffers.erase(buffers.begin() + bufferIndex);
      if (bufferIndex >= int(buffers.size())) { bufferIndex -= 1; }
      if (bufferIndex < 0) {
        view.setBuffer(nullptr);
      } else {
        view.setBuffer(buffers[bufferIndex]);
      }
    }
  };
  shortcuts.set({.key = SDLK_w, .ctrl = true}, closeFile);
  shortcuts.set({.key = SDLK_F4, .ctrl = true}, closeFile);
  shortcuts.set({.key = SDLK_s, .ctrl = true}, [&] {
    if (buffers.empty() || bufferIndex < 0) return;
    if (view.getBuffer()->getFilename().empty()) {
      saveWithFileDialog(*view.getBuffer());
    } else {
      view.getBuffer()->save();
    }
  });
  shortcuts.set({.key = SDLK_s, .ctrl = true, .shift = true}, [&] {
    if (buffers.empty() || bufferIndex < 0) return;
    saveWithFileDialog(*view.getBuffer());
  });
  shortcuts.set({.key = SDLK_z, .ctrl = true}, [&] { view.undo(); });
  shortcuts.set({.key = SDLK_z, .ctrl = true, .shift = true},
                [&] { view.redo(); });

  // Swap colors
  shortcuts.set({.key = SDLK_x, .alt = true}, [&] { view.swapColors(); });
}
}

int
main(int argc, char** argv)
{
  try {
    if (SDL_Init(SDL_INIT_VIDEO)) throw std::runtime_error{SDL_GetError()};
    pixedit::EditorInitSettings settings{
      .windowSz =
        {
          pixedit::defaults::WINDOW_WIDTH,
          pixedit::defaults::WINDOW_HEIGHT,
        },
      .filename = "../assets/samples/redball_128x128.png",
    };
    if (argc > 1) { settings.filename = argv[argc - 1]; }

    pixedit::EditorApp app{settings};
    return app.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
    std::cerr << "Unknown error\n";
  }
  return EXIT_FAILURE;
}
