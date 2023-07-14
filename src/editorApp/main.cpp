#include <iostream>
#include <map>
#include "EditorApp.hpp"
#include "ToolDescription.hpp"

namespace pixedit::defaults {
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const bool WINDOW_MAXIMIZED;
} // namespace pixedit::defaults

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
