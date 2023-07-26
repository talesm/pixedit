#ifndef PIXEDIT_SRC_EDITOR_APP_IMGUI_MAIN_MENU_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_IMGUI_MAIN_MENU_INCLUDED

#include <imgui.h>
#include "../actions.hpp"
#include "PictureView.hpp"
#include "tools.hpp"

namespace pixedit {

inline void
showFileMenuContent(bool hasBuffer)
{
  if (ImGui::MenuItem("New")) { pushAction(actions::PIC_NEW); }
  if (ImGui::MenuItem("Open", "Ctrl+O")) { pushAction(actions::PIC_OPEN); }
  if (ImGui::MenuItem("Save", "Ctrl+S")) { pushAction(actions::PIC_SAVE); }
  if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
    pushAction(actions::PIC_SAVE_AS);
  }
  if (ImGui::MenuItem("Close File", "Ctrl+F4", nullptr, hasBuffer)) {
    pushAction(actions::PIC_CLOSE);
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Exit")) { pushAction(actions::QUIT); }
}

inline void
showEditMenuContent()
{
  if (ImGui::MenuItem("Undo", "Ctrl+Z")) { pushAction(actions::HISTORY_UNDO); }
  if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z")) {
    pushAction(actions::HISTORY_REDO);
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Cut", "Ctrl+X")) { pushAction(actions::CLIP_CUT); }
  if (ImGui::MenuItem("Copy", "Ctrl+C")) { pushAction(actions::CLIP_COPY); }
  if (ImGui::MenuItem("Paste", "Ctrl+V")) { pushAction(actions::CLIP_PASTE); }
  if (ImGui::MenuItem("Paste as new", "Ctrl+Shift+V")) {
    pushAction(actions::CLIP_PASTE_NEW);
  }
}

inline void
showMainMenuBar(PictureView& view, bool* maximizeView)
{
  auto buffer = view.getBuffer();
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      showFileMenuContent(!!buffer);
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      showEditMenuContent();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
      ImGui::Checkbox("Maximize", maximizeView);
      bool gridEnabled = view.isGridEnabled();
      if (ImGui::Checkbox("Show grid", &gridEnabled)) {
        pushAction(actions::VIEW_GRID_TOGGLE);
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("About")) {
        pushAction(actions::MODAL_SHOW, "About");
      }
      ImGui::EndMenu();
    }
    ImGui::Dummy({50, 0});
    ImGui::Text("%s %dx%d",
                getTool(view.getToolId()).name.c_str(),
                view.state.x,
                view.state.y);
    if (buffer) {
      ImGui::Dummy({20, 0});
      ImGui::Text("%dx%d", buffer->getW(), buffer->getH());
      ImGui::Text("%.2f%%", view.scale * 100);
    }
    ImGui::EndMainMenuBar();
  }
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_IMGUI_MAIN_MENU_INCLUDED */
