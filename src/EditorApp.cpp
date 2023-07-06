#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include "FileDialogTinyfd.hpp"
#include "ImGuiAppBase.hpp"
#include "PngXClip.hpp"
#include "tools/FreeHandTool.hpp"
#include "tools/LinesTool.hpp"
#include "tools/RectTool.hpp"
#include "tools/ZoomTool.hpp"

namespace pixedit {

class EditorApp : ImGuiAppBase
{
public:
  EditorApp(InitSettings settings = {});
  using ImGuiAppBase::run;

private:
  void setupShortcuts();

  void showPictureOptions() final
  {
    ImGuiAppBase::showPictureOptions();

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
};

EditorApp::EditorApp(InitSettings settings)
  : ImGuiAppBase(settings)
{
  setupShortcuts();

  tools.emplace_back("Pan", [] { return nullptr; });
  tools.emplace_back("Zoom", [] { return new ZoomTool{}; });
  tools.emplace_back("Free hand", [] { return new FreeHandTool{}; });
  tools.emplace_back("Lines", [] { return new LinesTool{}; });
  tools.emplace_back("Outline Rect", [] { return new RectTool{true}; });
  tools.emplace_back("Filled Rect", [] { return new RectTool{false}; });

  view.canvas | ColorA{0, 0, 0, 255};
  view.canvas | ColorB{255, 255, 255, 255};
}

void
EditorApp::setupShortcuts()
{
  shortcuts.set({.key = SDLK_o, .ctrl = true}, [&] {
    auto buffer = loadFromFileDialog("./");
    if (buffer) { appendFile(buffer); };
  });
  shortcuts.set({.key = SDLK_c, .ctrl = true}, [&] {
    if (view.getBuffer()) copyToXClip(view.getBuffer()->getSurface());
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
    pixedit::InitSettings settings;
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
