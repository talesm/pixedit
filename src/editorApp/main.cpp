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
    if (SDL_Init(SDL_INIT_VIDEO)) throw std::runtime_error{SDL_GetError()};
    EditorInitSettings settings{
      .windowSz =
        {
          defaults::WINDOW_WIDTH,
          defaults::WINDOW_HEIGHT,
        },
      .filename = defaults::INITIAL_FILENAME,
      .pictureSz = {defaults::INITIAL_SIZE[0], defaults::INITIAL_SIZE[1]}};
    if (argc > 1) { settings.filename = argv[argc - 1]; }

    EditorApp app{settings};
    return app.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
    std::cerr << "Unknown error\n";
  }
  return EXIT_FAILURE;
}
