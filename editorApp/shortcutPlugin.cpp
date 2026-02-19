#include "shortcutPlugin.hpp"
#include "actions.hpp"

namespace pixedit {

std::any
shortcutDefaultsPlugin(PluginContext& ctx)
{
  AddShortcutCapability addShortcut{ctx};
  addShortcut({.key = SDLK_N, .ctrl = true}, actions::PIC_NEW);
  addShortcut({.key = SDLK_O, .ctrl = true}, actions::PIC_OPEN);
  addShortcut({.key = SDLK_ESCAPE}, actions::SELECTION_PERSIST);
  addShortcut({.key = SDLK_DELETE}, actions::SELECTION_DELETE);
  addShortcut({.key = SDLK_C, .ctrl = true}, actions::CLIP_COPY);
  addShortcut({.key = SDLK_X, .ctrl = true}, actions::CLIP_CUT);
  addShortcut({.key = SDLK_V, .ctrl = true}, actions::CLIP_PASTE);
  addShortcut({.key = SDLK_V, .ctrl = true, .shift = true},
              actions::CLIP_PASTE_NEW);
  addShortcut({.key = SDLK_W, .ctrl = true}, actions::PIC_CLOSE);
  addShortcut({.key = SDLK_F4, .ctrl = true}, actions::PIC_CLOSE);
  addShortcut({.key = SDLK_S, .ctrl = true}, actions::PIC_SAVE);
  addShortcut({.key = SDLK_S, .ctrl = true, .shift = true},
              actions::PIC_SAVE_AS);
  addShortcut({.key = SDLK_Z, .ctrl = true}, actions::HISTORY_UNDO);
  addShortcut({.key = SDLK_Z, .ctrl = true, .shift = true},
              actions::HISTORY_REDO);
  // Swap colors
  addShortcut({.key = SDLK_X, .alt = true}, actions::EDITOR_COLOR_SWAP);

  // grid
  addShortcut({.key = SDLK_G, .alt = true}, actions::VIEW_GRID_TOGGLE);
  return nullptr;
}
} // namespace pixedit
