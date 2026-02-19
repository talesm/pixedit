#include <iostream>
#include <map>
#include "EditorApp.hpp"
#include "ToolDescription.hpp"

namespace pixedit::defaults {
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const bool WINDOW_MAXIMIZED;
extern const char INITIAL_FILENAME[];
extern const int INITIAL_SIZE[2];
} // namespace pixedit::defaults

int
main(int argc, char** argv)
{
  using namespace pixedit;
  try {
    EditorInitSettings settings{
      .windowSz =
        {
          defaults::WINDOW_WIDTH,
          defaults::WINDOW_HEIGHT,
        },
      .filename = defaults::INITIAL_FILENAME,
      .pictureSz = {defaults::INITIAL_SIZE[0], defaults::INITIAL_SIZE[1]}};
    if (argc > 1) { settings.filename = argv[argc - 1]; }
    auto editorApp = createEditorApp(settings);

    SDL::AppResult result = SDL::APP_CONTINUE;
    while (result == SDL::APP_CONTINUE) {
      for (SDL_Event ev; SDL_PollEvent(&ev);) {
        if (auto r = editorApp->Event(ev); r != SDL::APP_CONTINUE) {
          result = r;
          goto exit;
        }
      }
      result = editorApp->Iterate();
      SDL::Delay(10);
    }
  exit:
    return result == SDL::APP_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
  } catch (std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
    std::cerr << "Unknown error\n";
  }
  return EXIT_FAILURE;
}
