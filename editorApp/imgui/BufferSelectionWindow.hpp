#ifndef PIXEDIT_SRC_EDITOR_APP_IMGUI_BUFFER_SELECTION_WINDOW_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_IMGUI_BUFFER_SELECTION_WINDOW_INCLUDED

#include <memory>
#include <vector>
#include "../actions.hpp"
#include "AuxWindowManager.hpp"
#include "PictureBuffer.hpp"

namespace pixedit {

std::shared_ptr<PictureBuffer>
currentBuffer();

inline AuxWindow
initBufferSelectionAuxWindow(
  const std::vector<std::shared_ptr<PictureBuffer>>* buffers,
  const bool& maximizeView)
{
  return [=]() {
    static std::string title = "Files";
    auto buffer = currentBuffer();
    if (!buffer || maximizeView || buffers->size() < 2) { return; }
    if (ImGui::Begin(title.c_str())) {
      if (ImGui::BeginCombo("File",
                            buffer->getFilename().empty()
                              ? "New File"
                              : buffer->getFilename().c_str())) {
        int i = 0;
        for (auto& b : *buffers) {
          bool selected = b == buffer;
          if (ImGui::Selectable(b->getFilename().c_str(), selected)) {
            pushAction(actions::VIEW_CHANGE, std::to_string(i));
          }
          if (selected) { ImGui::SetItemDefaultFocus(); }
          ++i;
        }
        ImGui::EndCombo();
      }
    }
    ImGui::End();
  };
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_IMGUI_BUFFER_SELECTION_WINDOW_INCLUDED */
