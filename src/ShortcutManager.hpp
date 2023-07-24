#ifndef PIXEDIT_SRC_SHORTCUT_MANAGER_INCLUDED
#define PIXEDIT_SRC_SHORTCUT_MANAGER_INCLUDED

#include <map>
#include <optional>
#include <ostream>
#include <SDL_keyboard.h>
#include "Id.hpp"

namespace pixedit {

struct Shortcut
{
  Sint32 key;
  bool ctrl = false;
  bool alt = false;
  bool shift = false;
};

constexpr bool
operator<(const Shortcut& lhs, const Shortcut& rhs)
{
  if (lhs.key != rhs.key) return lhs.key < rhs.key;
  if (lhs.ctrl != rhs.ctrl) return rhs.ctrl;
  if (lhs.alt != rhs.alt) return rhs.alt;
  return !lhs.shift && rhs.shift;
}

inline std::ostream&
operator<<(std::ostream& out, const Shortcut& sc)
{
  if (sc.ctrl) out << "Ctrl+";
  if (sc.alt) out << "Alt+";
  if (sc.shift) out << "Shift+";
  return out << SDL_GetKeyName(sc.key);
}

struct ShortcutManager
{
  std::map<Shortcut, IdOwn> shortcuts;

  void set(Shortcut sc, Id action) { shortcuts[sc] = IdOwn{action}; }
  void unset(Shortcut sc) { shortcuts.erase(sc); }
  std::optional<Id> get(Shortcut sc) const
  {
    auto it = shortcuts.find(sc);
    if (it == shortcuts.end()) return {};
    return it->second;
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_SHORTCUT_MANAGER_INCLUDED */
