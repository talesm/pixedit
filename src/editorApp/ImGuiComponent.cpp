#include "ImGuiComponent.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "utils/paths.hpp"

namespace pixedit {

ImGuiComponent::ImGuiComponent(SDL_Window* window, SDL_Renderer* renderer):renderer(renderer)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  static std::string defaultIni = getPrefPath() + "imgui.ini";
  io.IniFilename = defaultIni.c_str();
  // io.ConfigFlags |=
  //   ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
    ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
}
ImGuiComponent::~ImGuiComponent()
{
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext(nullptr);
}

bool
ImGuiComponent::event(const SDL_Event& ev)
{
  return ImGui_ImplSDL3_ProcessEvent(&ev);
}

void
ImGuiComponent::update()
{
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
#ifndef NDEBUG
  ImGui::ShowDemoWindow();
#endif // NDEBUG
}

void
ImGuiComponent::render()
{
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

} // namespace pixedit
