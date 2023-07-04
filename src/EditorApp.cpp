#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include "FileDialogTinyfd.hpp"
#include "ImGuiAppBase.hpp"
#include "PngXClip.hpp"
#include "tools/FreeHandTool.hpp"
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
    auto colorA = view.canvas.getColorANormalized();
    if (ImGui::ColorEdit4(
          "Color A", colorA.data(), ImGuiColorEditFlags_NoInputs)) {
      view.canvas |
        ColorA{
          colorA[0] * 255, colorA[1] * 255, colorA[2] * 255, colorA[3] * 255};
    }
    auto colorB = view.canvas.getColorBNormalized();
    if (ImGui::ColorEdit4(
          "Color B", colorB.data(), ImGuiColorEditFlags_NoInputs)) {
      view.canvas |
        ColorB{
          colorB[0] * 255, colorB[1] * 255, colorB[2] * 255, colorB[3] * 255};
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
  shortcuts.set({.key = SDLK_s, .ctrl = true}, [&] {
    if (buffers.empty() || bufferIndex < 0) return;
    if (view.buffer->filename.empty()) {
      saveWithFileDialog(view.buffer->filename.c_str(), *view.buffer);
    } else {
      view.buffer->save();
    }
  });
  shortcuts.set({.key = SDLK_s, .ctrl = true, .shift = true}, [&] {
    if (buffers.empty() || bufferIndex < 0) return;
    saveWithFileDialog(view.buffer->filename.c_str(), *view.buffer);
  });
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
