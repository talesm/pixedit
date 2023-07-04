#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "ImGuiAppBase.hpp"
#include "PngXClip.hpp"
#include "ZoomTool.hpp"
#include "tinyfiledialogs.h"

namespace pixedit {

class ViewerApp : ImGuiAppBase
{
public:
  ViewerApp(InitSettings settings = {});
  using ImGuiAppBase::run;

private:
  void setupShortcuts();
};

ViewerApp::ViewerApp(InitSettings settings)
  : ImGuiAppBase(settings)
{
  setupShortcuts();

  tools.emplace_back("Pan", [] { return nullptr; });
  tools.emplace_back("Zoom", [] { return new ZoomTool{}; });
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
    if (filename) { loadFile(filename); }
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
