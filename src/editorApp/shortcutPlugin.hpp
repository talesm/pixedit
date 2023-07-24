#ifndef PIXEDIT_SRC_EDITOR_APP_SHORTCUT_PLUGIN_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_SHORTCUT_PLUGIN_INCLUDED

#include "Id.hpp"
#include "PluginManager.hpp"
#include "ShortcutManager.hpp"

namespace pixedit {

namespace plugins {
constexpr Id shortcut{"core.shortcut"};
constexpr Id shortcutDefaults{"core.shortcut.defaults"};

} // namespace plugins

// TODO move to its own file?
class AddShortcutCapability
{
private:
  PluginContext& ctx;
  ShortcutManager* shortcuts = nullptr;

public:
  AddShortcutCapability(PluginContext& ctx)
    : ctx(ctx)
  {
    shortcuts = std::any_cast<ShortcutManager*>(ctx.require(plugins::shortcut));
  }

  void operator()(Shortcut shortcut, Id action)
  {
    shortcuts->set(shortcut, action);
    ctx.onRemove(
      [shortcut, shortcuts = this->shortcuts] { shortcuts->unset(shortcut); });
  }
};

inline Id
installShortcutPlugin(PluginManager& plugins, ShortcutManager& shortcuts)
{
  plugins.set(plugins::shortcut, [&](auto&) { return &shortcuts; });
  return plugins::shortcut;
}

std::any
shortcutDefaultsPlugin(PluginContext& ctx);

inline Id
installShortcutDefaultsPlugin(PluginManager& plugins)
{
  plugins.set(plugins::shortcutDefaults, shortcutDefaultsPlugin);
  return plugins::shortcutDefaults;
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_SHORTCUT_PLUGIN_INCLUDED */
