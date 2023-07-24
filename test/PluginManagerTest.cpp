#include "catch.hpp"
#include "PluginManager.hpp"

using namespace pixedit;

TEST_CASE("PluginManager")
{
  PluginManager manager;
  REQUIRE_FALSE(manager.has("a"));
  int a = 13;
  manager.set("a", [&](auto& ctx) { return a; });
  REQUIRE(manager.has("a"));

  int b = 0;
  REQUIRE_NOTHROW(manager.set("b", [&](auto& ctx) {
    b = std::any_cast<int>(ctx.require("a"));
    return &b;
  }));
  REQUIRE_NOTHROW(manager.set("c", [](auto& ctx) {
    int& b = *std::any_cast<int*>(ctx.require("b"));
    b = 19;
    return true;
  }));
  REQUIRE(b == 19);
  REQUIRE_THROWS(manager.set("d", [](auto& ctx) {
    ctx.require("e");
    return true;
  }));
}
