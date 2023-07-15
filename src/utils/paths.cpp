#include "paths.hpp"

namespace pixedit {

namespace defaults {

extern const char ORG_NAME[];
extern const char APP_NAME[];
} // namespace defaults

/**
 * Base path where app, but might be Read only
 */
const std::string&
getBasePath()
{
  static const std::string path = [] {
    auto s = SDL_GetBasePath();
    std::string str{s};
    SDL_free(s);
    return str;
  }();
  return path;
}

/**
 * Preferred path for storing configuration
 */
const std::string&
getPrefPath()
{
  static const std::string path = [] {
    auto s = SDL_GetPrefPath(defaults::ORG_NAME, defaults::APP_NAME);
    std::string str{s};
    SDL_free(s);
    return str;
  }();
  return path;
}

} // namespace pixedit
