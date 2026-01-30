#include "ImGuiComponent.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
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
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);
}
ImGuiComponent::~ImGuiComponent()
{
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext(nullptr);
}

bool
ImGuiComponent::event(const SDL_Event& ev)
{
  return ImGui_ImplSDL2_ProcessEvent(&ev);
}

void
ImGuiComponent::update()
{
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
#ifndef NDEBUG
  ImGui::ShowDemoWindow();
#endif // NDEBUG
}

void
ImGuiComponent::render()
{
  ImGui::Render();
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

} // namespace pixedit
