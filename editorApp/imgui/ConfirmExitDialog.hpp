#ifndef PIXEDIT_SRC_EDITOR_APP_IMGUI_CONFIRM_EXIT_DIALOG_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_IMGUI_CONFIRM_EXIT_DIALOG_INCLUDED

#include <memory>
#include <imgui.h>
#include "../ViewSettings.hpp"
#include "../actions.hpp"
#include "PictureView.hpp"

namespace pixedit {

std::shared_ptr<PictureBuffer>
currentBuffer();

inline void
showConfirmExitDialog(bool* exiting)
{
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (ImGui::BeginPopupModal(
        "Confirm exit", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    auto buffer = currentBuffer();
    ImGui::Text("Image unsaved changes will be lost?");
    const char* cstr = "";
    if (buffer) { cstr = getSettingsFor(buffer).filename.c_str(); }
    ImGui::Text("File: %s", cstr);
    ImGui::Separator();

    if (ImGui::Button("Close",
                      ImVec2(ImGui::GetContentRegionAvail().x * .45f, 0))) {
      pushAction(actions::PIC_FORCE_CLOSE);
      if (*exiting) {
        pushAction(actions::PIC_CLOSE);
      } else {
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
      ImGui::CloseCurrentPopup();
      *exiting = false;
    }
    ImGui::SetItemDefaultFocus();
    ImGui::EndPopup();
  } else if (*exiting) {
    pushAction(actions::PIC_CLOSE);
  }
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_IMGUI_CONFIRM_EXIT_DIALOG_INCLUDED */
