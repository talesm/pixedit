#ifndef PIXEDIT_SRC_EDITOR_APP_IMGUI_NEW_FILE_DIALOG_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_IMGUI_NEW_FILE_DIALOG_INCLUDED

#include <imgui.h>

namespace pixedit {

void
appendFile(std::shared_ptr<PictureBuffer> buffer);

inline void
showNewFileDialog()
{
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (ImGui::BeginPopupModal(
        "New image", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Do you want to create a new image");
    ImGui::Separator();

    static int width = 320, height = 240;
    if (ImGui::InputInt("width", &width)) {
      if (width < 1) width = 1;
    }
    if (ImGui::InputInt("height", &height)) {
      if (height < 1) height = 1;
    }

    if (ImGui::Button("OK", ImVec2(120, 0))) {
      appendFile(
        std::make_shared<PictureBuffer>("", Surface::create(width, height)));
      ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
    ImGui::EndPopup();
  }
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_IMGUI_NEW_FILE_DIALOG_INCLUDED */
