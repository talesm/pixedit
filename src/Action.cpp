#include "Action.hpp"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace pixedit {

static std::vector<ActionManager*>&
getManagers()
{
  static std::vector<ActionManager*> managers;
  return managers;
}

static void
prepareEvent(Uint32 type, Sint32 code, std::string_view param)
{
  SDL_Event ev;
  SDL_zero(ev);
  ev.type = type;
  ev.user.code = code;
  if (!param.empty()) {
    auto strParam = new char[param.size() + 1];
    std::copy(param.begin(), param.end(), strParam);
    strParam[param.size()] = 0;
    ev.user.data1 = strParam;
  }
  SDL_PushEvent(&ev);
}

void
pushAction(Id action, Id parameter)
{
  std::stringstream ss;
  ss << action << ':' << parameter;
  pushAction(ss.str());
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
      prepareEvent(m->type, it->second.code, {});
      return;
    }
  }
  for (auto colonPos = action.find_last_of(':');
       colonPos != std::string::npos && colonPos > 0;
       colonPos = action.find_last_of(':', colonPos - 1)) {
    for (auto m : getManagers()) {
      auto it = m->actions.find(action.substr(0, colonPos));
      if (it != m->actions.end()) {
        prepareEvent(m->type, it->second.code, action.substr(colonPos + 1));
        return;
      }
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
  set(id, [action = std::move(action)](auto _) { action(); });
}

void
ActionManager::set(Id id, ActionWithParameter action)
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
      auto strParam = static_cast<char*>(ev.data1);
      auto id = m->codeToId.at(ev.code);
      m->actions[id].action(strParam ?: Id{});
      delete strParam;
      return true;
    }
  }
  return false;
}

} // namespace pixedit
