#ifndef PIXEDIT_SRC_EDITOR_APP_IMGUI_AUX_WINDOW_MANAGER_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_IMGUI_AUX_WINDOW_MANAGER_INCLUDED

#include <functional>
#include <unordered_map>
#include "Id.hpp"

namespace pixedit {

/// @brief AuxWindow handler
using AuxWindow = std::function<void()>;

class AuxWindowManager
{
  struct AuxWindowDescription
  {
    IdOwn id;
    AuxWindow handler;
    bool enabled;
  };

  std::unordered_map<IdRef, AuxWindowDescription> windows;

public:
  void set(Id id, AuxWindow window, bool enabled = true)
  {
    windows.emplace(id, AuxWindowDescription{IdOwn{id}, window, enabled});
  }

  void unset(Id id) { windows.erase(id); }

  bool isEnabled(Id id) const { return windows.contains(id); }

  void enable(Id id, bool enabled = true) { windows.at(id).enabled = enabled; }

  void showAll() const
  {
    for (auto& win : windows) {
      if (!win.second.enabled) continue;
      win.second.handler();
    }
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_IMGUI_AUX_WINDOW_MANAGER_INCLUDED */
