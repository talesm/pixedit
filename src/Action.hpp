#ifndef PIXEDIT_SRC_ACTION_INCLUDED
#define PIXEDIT_SRC_ACTION_INCLUDED

#include <functional>
#include <unordered_map>
#include <SDL.h>
#include "Id.hpp"

namespace pixedit {

namespace actions {
constexpr Id QUIT{"quit"};

} // namespace actions

using Action = std::function<void()>;
using ActionWithParameter = std::function<void(std::string_view)>;

/**
 * Request action to be performed
 */
void
pushAction(Id action);

void
pushAction(Id action, Id parameter);

class ActionManager
{
  Uint32 type;

  struct ActionDescription
  {
    IdOwn name;
    ActionWithParameter action;
    Sint32 code;
  };

  std::unordered_map<IdRef, ActionDescription> actions;
  std::unordered_map<Sint32, IdRef> codeToId;

public:
  ActionManager(Uint32 eventType);
  ActionManager(const ActionManager&) = delete;
  ActionManager(ActionManager&& rhs);
  ~ActionManager();
  ActionManager& operator=(ActionManager rhs);

  void set(Id id, Action action);
  void set(Id id, ActionWithParameter action);
  void unset(Id id);

  static bool check(const SDL_UserEvent& ev);

  friend void pushAction(Id action);
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_ACTION_INCLUDED */
