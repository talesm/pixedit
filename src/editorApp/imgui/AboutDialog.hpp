#ifndef PIXEDIT_SRC_EDITOR_APP_IMGUI_ABOUT_DIALOG_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_IMGUI_ABOUT_DIALOG_INCLUDED

#include <imgui.h>

namespace pixedit {

void
showAboutDialog()
{
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (ImGui::BeginPopupModal(
        "About", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Pixedit version: v0.1.0-AuspiciousAmber (Alpha)");
    ImGui::Separator();
    ImGui::TextWrapped(
      "A simple and customizable pixel art focused image editor");
    ImGui::Text("Homepage: ");
    ImGui::SameLine();
    static const char url[] = "https://github.com/jungleowl2/pixedit";
    if (ImGui::SmallButton(url)) { ImGui::SetClipboardText(url); }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) &&
        ImGui::BeginTooltip()) {
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted("Click to copy url to clipboard");
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }

    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("Ok", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::EndPopup();
  }
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_IMGUI_ABOUT_DIALOG_INCLUDED */
