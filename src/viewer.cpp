#include <iostream>
#include <SDL.h>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

int
main()
{
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("Pixedit viewer",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            800,
                            600,
                            SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, -1, 0);

  bool exited = false;
  while (!exited) {
    for (SDL_Event ev; SDL_PollEvent(&ev);) {
      switch (ev.type) {
      case SDL_QUIT: exited = true; break;
      default: break;
      }
    }
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
    SDL_Delay(0);
  }
  return 0;
}
