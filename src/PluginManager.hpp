#ifndef PIXEDIT_SRC_PLUGIN_MANAGER_INCLUDED
#define PIXEDIT_SRC_PLUGIN_MANAGER_INCLUDED

#include <any>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Id.hpp"

namespace pixedit {

// Forward declare
class PluginManager;

struct NotAvailableError : std::runtime_error
{
  IdOwn id;

  NotAvailableError(Id id)
    : std::runtime_error{"Capability not available" + std::string{id}}
    , id{id}
  {
  }
};

class PluginContext
{
  PluginManager& manager;
  Id id;
  std::vector<std::function<void()>>& removeListeners;
  std::vector<std::function<void()>>& installListeners;

public:
  PluginContext(PluginManager& manager,
                Id id,
                std::vector<std::function<void()>>& removeListeners,
                std::vector<std::function<void()>>& installListeners)
    : manager{manager}
    , id{id}
    , removeListeners{removeListeners}
    , installListeners{installListeners}
  {
  }

  Id getId() const { return id; }

  void onRemove(auto func) { removeListeners.push_back(func); }

  void onInstall(auto func) { installListeners.push_back(func); }

  /// @brief Require plugin as dependency
  /// @param id plugin id
  /// @return data associated with it
  /// @throws NotAvailableError if not found or not authorized
  std::any require(Id id);
};

using Plugin = std::function<std::any(PluginContext& ctx)>;

class PluginManager
{
  struct PluginDescription
  {
    IdOwn id;
    std::any data;
    std::unordered_set<IdOwn> dependents;
    std::vector<std::function<void()>> removeListeners;
  };

  std::unordered_map<IdRef, PluginDescription> plugins;

public:
  ~PluginManager();

  void set(Id id, Plugin plugin);
  void unset(Id id);
  bool has(Id id) const { return plugins.contains(id); }

  friend class PluginContext;
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PLUGIN_MANAGER_INCLUDED */
