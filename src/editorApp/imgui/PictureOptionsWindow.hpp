#ifndef PIXEDIT_SRC_EDITOR_APP_IMGUI_PICTURE_OPTIONS_WINDOW_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_IMGUI_PICTURE_OPTIONS_WINDOW_INCLUDED

#include <map>
#include <memory>
#include <vector>
#include "../actions.hpp"
#include "AuxWindowManager.hpp"
#include "PictureManager.hpp"
#include "PictureView.hpp"
#include "tools.hpp"

namespace pixedit {

PictureView&
currentView();

PictureManager&
currentPicture();

inline bool
PatternCombo(const char* label, Pattern* pattern)
{
  static std::map<Uint64, const char*> pNames{
    {patterns::SOLID.data8x8, "Solid"},
    {patterns::CHECKERED.data8x8, "Checkered"},
    {patterns::CHECKERED_2.data8x8, "Checkered 2x2"},
    {patterns::CHECKERED_4.data8x8, "Checkered 4x4"},
  };
  bool result = false;
  if (ImGui::BeginCombo(label, pNames[pattern->data8x8] ?: "????")) {
    for (auto& e : pNames) {
      bool isSelected = e.first == pattern->data8x8;
      if (ImGui::Selectable(e.second, isSelected)) {
        pattern->data8x8 = e.first;
        result = true;
      }
      if (isSelected) { ImGui::SetItemDefaultFocus(); }
    }
    ImGui::EndCombo();
  }
  return result;
}

void
pictureOptionsAuxWindow()
{
  if (ImGui::Begin("Picture options")) {
    auto& picture = currentPicture();
    if (ImGui::CollapsingHeader("Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto currId = picture.getToolId();
      for (auto& tool : getTools()) {
        if (ImGui::RadioButton(tool.name.c_str(), tool.id == currId)) {
          picture.setToolId(tool.id);
          pushAction(actions::EDITOR_FOCUS_PICTURE);
        }
      }
    }
    ImGui::DragFloat2("##offset", &currentView().offset.x, 1.f, -10000, +10000);
    ImGui::SameLine();
    if (ImGui::Button("Reset##offset")) { currentView().offset = {0}; }
    if (ImGui::ArrowButton("Decrease zoom", ImGuiDir_Left)) {
      currentView().scale /= 2;
      pushAction(actions::EDITOR_FOCUS_PICTURE);
    }
    ImGui::SameLine();
    ImGui::Text("%g%%", currentView().scale * 100);
    ImGui::SameLine();
    if (ImGui::ArrowButton("Increase zoom", ImGuiDir_Right)) {
      currentView().scale *= 2;
      pushAction(actions::EDITOR_FOCUS_PICTURE);
    }

    float colorAreaHeight =
      ImGui::GetFrameHeightWithSpacing() + ImGui::GetFrameHeight();
    if (ImGui::Button("Swap colors", {0, colorAreaHeight})) {
      pushAction(actions::EDITOR_COLOR_SWAP);
      pushAction(actions::EDITOR_FOCUS_PICTURE);
    }
    ImGui::SameLine();
    {
      ImGui::BeginChild("Colors", {0, colorAreaHeight});
      auto colorA = componentToNormalized(picture.getColorA());
      if (ImGui::ColorEdit4(
            "Color A", colorA.data(), ImGuiColorEditFlags_NoInputs)) {
        picture.setColorA(normalizedToComponent(colorA));
      }
      auto colorB = componentToNormalized(picture.getColorB());
      if (ImGui::ColorEdit4(
            "Color B", colorB.data(), ImGuiColorEditFlags_NoInputs)) {
        picture.setColorA(normalizedToComponent(colorB));
      }
      ImGui::EndChild();
    }
    auto brush = picture.getBrush();
    int penSize = brush.pen.w;
    if (ImGui::SliderInt("Pen size", &penSize, 1, 16)) {
      brush.pen = Pen{penSize, penSize};
      picture.setBrush(brush);
    }
    Pattern pattern = brush.pattern;
    if (PatternCombo("Tile: ", &pattern)) {
      brush.pattern = pattern;
      picture.setBrush(brush);
      pushAction(actions::EDITOR_FOCUS_PICTURE);
    }
    if (ImGui::CollapsingHeader("Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
      bool transparent = picture.isTransparent();
      if (ImGui::Checkbox("Transparent", &transparent)) {
        picture.setTransparent(transparent);
        pushAction(actions::EDITOR_FOCUS_PICTURE);
      }
      ImGui::Checkbox("Fill selected out region",
                      &currentView().fillSelectedOut);
    }
  }
  ImGui::End();
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_IMGUI_PICTURE_OPTIONS_WINDOW_INCLUDED */
