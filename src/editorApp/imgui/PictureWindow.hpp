#ifndef PIXEDIT_SRC_EDITOR_APP_IMGUI_PICTURE_WINDOW_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_IMGUI_PICTURE_WINDOW_INCLUDED

#include <memory>
#include "../ViewSettings.hpp"
#include "PictureBuffer.hpp"

namespace pixedit {

PictureManager&
currentPicture();

inline void
showPictureWindow(SDL_Renderer* renderer,
                  const std::shared_ptr<PictureBuffer>& buffer)
{
  auto& settings = getSettingsFor(buffer);
  ImGuiWindowFlags flags = 0;
  if (buffer->isDirty()) { flags |= ImGuiWindowFlags_UnsavedDocument; }
  // TODO Measure title and decoration instead of guessing
  ImGui::SetNextWindowSize(ImVec2(buffer->getW() + 16, buffer->getH() + 35),
                           ImGuiCond_Once);
  bool stayOpen = true;
  if (ImGui::Begin(settings.titleBuffer.c_str(), &stayOpen, flags)) {
    bool redraw = false;
    ImVec2 canvasSz = ImGui::GetContentRegionAvail();
    if (canvasSz.x < 50.0f) canvasSz.x = 50.0f;
    if (canvasSz.y < 50.0f) canvasSz.y = 50.0f;
    {
      int picW, picH;
      SDL_QueryTexture(settings.texture, nullptr, nullptr, &picW, &picH);
      if (settings.texture == nullptr || picW != canvasSz.x ||
          picH != canvasSz.y) {
        SDL_DestroyTexture(settings.texture);
        settings.texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ABGR32,
                                             SDL_TEXTUREACCESS_TARGET,
                                             canvasSz.x,
                                             canvasSz.y);
        SDL_SetTextureBlendMode(settings.texture, SDL_BLENDMODE_BLEND);
        redraw = true;
      }
    }
    ImVec2 canvasP0 = ImGui::GetCursorScreenPos();
    ImVec2 canvasP1 = ImVec2(canvasP0.x + canvasSz.x, canvasP0.y + canvasSz.y);

    ImGuiIO& io = ImGui::GetIO();

    auto& view = settings.view;
    if (ImGui::IsWindowFocused()) {
      ImGui::InvisibleButton("Canvas",
                             canvasSz,
                             ImGuiButtonFlags_MouseButtonLeft |
                               ImGuiButtonFlags_MouseButtonRight |
                               ImGuiButtonFlags_MouseButtonMiddle);
      const bool is_hovered = ImGui::IsItemHovered(); // Hovered
      const bool is_active = ImGui::IsItemActive();   // Held
      if (is_hovered || is_active) {
        view.state.x = io.MousePos.x - canvasP0.x;
        view.state.y = io.MousePos.y - canvasP0.y;
        view.state.left = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        view.state.middle = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
        view.state.right = ImGui::IsMouseDown(ImGuiMouseButton_Right);
        view.state.wheelX += io.MouseWheelH;
        view.state.wheelY += io.MouseWheel;
      }
    }
    if (ImGui::IsWindowFocused() || redraw) {
      view.setBuffer(buffer);
      SDL_SetRenderTarget(renderer, settings.texture);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      auto& picture = currentPicture();
      picture.update(&view, renderer, {0, 0, int(canvasSz.x), int(canvasSz.y)});
      picture.render(&view, renderer, {0, 0, int(canvasSz.x), int(canvasSz.y)});

      SDL_SetRenderTarget(renderer, nullptr);
    }
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddImage(settings.texture, canvasP0, canvasP1);
  }
  if (!stayOpen) { pushAction(actions::PIC_CLOSE); }
  ImGui::End();
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_IMGUI_PICTURE_WINDOW_INCLUDED */
