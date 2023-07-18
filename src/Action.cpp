#include "Action.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace pixedit {

static std::vector<ActionManager*>&
getManagers()
{
  static std::vector<ActionManager*> managers;
  return managers;
}

void
pushAction(Id action)
{
  if (action == actions::QUIT) {
    SDL_Event ev;
    SDL_zero(ev);
    ev.type = SDL_QUIT;
    SDL_PushEvent(&ev);
    return;
  }
  for (auto m : getManagers()) {
    auto it = m->actions.find(action);
    if (it != m->actions.end()) {
      SDL_Event ev;
      SDL_zero(ev);
      ev.type = m->type;
      ev.user.code = it->second.code;
      SDL_PushEvent(&ev);
      return;
    }
  }
  throw std::runtime_error{"Invalid action" + std::string{action}};
}

ActionManager::ActionManager(Uint32 eventType)
  : type(eventType)
{
  getManagers().push_back(this);
}
ActionManager::ActionManager(ActionManager&& rhs)
  : type(rhs.type)
  , actions(std::move(rhs.actions))
  , codeToId(std::move(rhs.codeToId))
{
  auto& managers = getManagers();
  auto it = std::find(managers.begin(), managers.end(), this);
  *it = this;
}

ActionManager::~ActionManager()
{
  auto& managers = getManagers();
  auto it = std::find(managers.begin(), managers.end(), this);
  managers.erase(it);
}

ActionManager&
ActionManager::operator=(ActionManager rhs)
{
  std::swap(type, rhs.type);
  std::swap(actions, rhs.actions);
  std::swap(codeToId, rhs.codeToId);
  return *this;
}

void
ActionManager::set(Id id, Action action)
{
  Sint32 nextCode = 0;
  for (int i = 1; i != -1; ++i) {
    if (!codeToId.contains(i)) {
      nextCode = i;
      break;
    }
  }
  auto& desc = actions[id] = ActionDescription{
    .name = IdOwn{id},
    .action = std::move(action),
    .code = nextCode,
  };
  codeToId[nextCode] = desc.name;
}

void
ActionManager::unset(Id id)
{
  auto& desc = actions[id];
  codeToId.erase(desc.code);
  actions.erase(id);
}

bool
ActionManager::check(const SDL_UserEvent& ev)
{
  for (auto m : getManagers()) {
    if (ev.type == m->type) {
      auto id = m->codeToId.at(ev.code);
      m->actions[id].action();
      return true;
    }
  }
  return false;
}

} // namespace pixedit
