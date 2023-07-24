#include "shortcutPlugin.hpp"
#include "actions.hpp"

namespace pixedit {

std::any
shortcutDefaultsPlugin(PluginContext& ctx)
{
  AddShortcutCapability addShortcut{ctx};
  addShortcut({.key = SDLK_n, .ctrl = true}, actions::PIC_NEW);
  addShortcut({.key = SDLK_o, .ctrl = true}, actions::PIC_OPEN);
  addShortcut({.key = SDLK_ESCAPE}, actions::SELECTION_PERSIST);
  addShortcut({.key = SDLK_DELETE}, actions::SELECTION_DELETE);
  addShortcut({.key = SDLK_c, .ctrl = true}, actions::CLIP_COPY);
  addShortcut({.key = SDLK_x, .ctrl = true}, actions::CLIP_CUT);
  addShortcut({.key = SDLK_v, .ctrl = true}, actions::CLIP_PASTE);
  addShortcut({.key = SDLK_v, .ctrl = true, .shift = true},
              actions::CLIP_PASTE_NEW);
  addShortcut({.key = SDLK_w, .ctrl = true}, actions::PIC_CLOSE);
  addShortcut({.key = SDLK_F4, .ctrl = true}, actions::PIC_CLOSE);
  addShortcut({.key = SDLK_s, .ctrl = true}, actions::PIC_SAVE);
  addShortcut({.key = SDLK_s, .ctrl = true, .shift = true},
              actions::PIC_SAVE_AS);
  addShortcut({.key = SDLK_z, .ctrl = true}, actions::HISTORY_UNDO);
  addShortcut({.key = SDLK_z, .ctrl = true, .shift = true},
              actions::HISTORY_REDO);
  // Swap colors
  addShortcut({.key = SDLK_x, .alt = true}, actions::EDITOR_COLOR_SWAP);

  // grid
  addShortcut({.key = SDLK_g, .alt = true}, actions::VIEW_GRID_TOGGLE);
  return nullptr;
}
} // namespace pixedit
