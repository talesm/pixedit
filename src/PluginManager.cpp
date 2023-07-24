#include "PluginManager.hpp"

namespace pixedit {

std::any
PluginContext::require(Id id)
{
  auto it = manager.plugins.find(id);
  if (it == manager.plugins.end()) { throw NotAvailableError{id}; }
  it->second.dependents.emplace(getId());
  return it->second.data;
}

PluginManager::~PluginManager()
{
  while (!plugins.empty()) { unset(plugins.begin()->second.id); }
}

void
PluginManager::set(Id id, Plugin plugin)
{
  IdOwn idOwn{id};
  std::vector<std::function<void()>> removeListeners;
  std::vector<std::function<void()>> installListeners;
  PluginContext ctx{*this, idOwn, removeListeners, installListeners};
  auto data = plugin(ctx);
  for (auto cb : installListeners) { cb(); }
  plugins.emplace(id,
                  PluginDescription{
                    .id{std::move(idOwn)},
                    .data = std::move(data),
                    .removeListeners{std::move(removeListeners)},
                  });
}

void
PluginManager::unset(Id id)
{
  auto it = plugins.find(id);
  if (it == plugins.end()) { return; }
  auto dependents = std::move(it->second.dependents);
  for (auto& dep : dependents) { unset(dep); }
  plugins.erase(it);
}

} // namespace pixedit
