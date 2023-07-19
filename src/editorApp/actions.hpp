#ifndef PIXEDIT_SRC_EDITOR_APP_ACTIONS_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_ACTIONS_INCLUDED

#include "Id.hpp"

namespace pixedit::actions {

constexpr Id PIC_CLOSE{"picture.close"};
constexpr Id PIC_OPEN{"picture.open"};
constexpr Id PIC_NEW{"picture.new"};
constexpr Id PIC_SAVE{"picture.save"};
constexpr Id PIC_SAVE_AS{"picture.save_as"};

constexpr Id CLIP_COPY{"clipboard.copy"};
constexpr Id CLIP_CUT{"clipboard.cut"};
constexpr Id CLIP_PASTE{"clipboard.paste"};
constexpr Id CLIP_PASTE_NEW{"clipboard.paste_new"};

constexpr Id SELECTION_PERSIST{"selection.persist"};
constexpr Id SELECTION_DELETE{"selection.delete"};

constexpr Id HISTORY_UNDO{"history.undo"};
constexpr Id HISTORY_REDO{"history.redo"};

constexpr Id VIEW_GRID_TOGGLE{"view.grid.toggle"};

constexpr Id EDITOR_COLOR_SWAP{"editor.color.swap"};
constexpr Id EDITOR_FOCUS_PICTURE{"editor.focus.picture"};
} // namespace pixedit::actions

#endif /* PIXEDIT_SRC_EDITOR_APP_ACTIONS_INCLUDED */
