#ifndef PIXEDIT_SRC_EDITOR_APP_VIEW_SETTINGS_INCLUDED
#define PIXEDIT_SRC_EDITOR_APP_VIEW_SETTINGS_INCLUDED

#include <memory>
#include <string>
#include <SDL.h>
#include "PictureView.hpp"

namespace pixedit {

struct ViewSettings
{
  PictureView view;
  SDL_Texture* texture = nullptr;
  int fileUnamedId = 0;
  std::string filename;
  std::string titleBuffer;
};

ViewSettings&
getSettingsFor(const std::shared_ptr<PictureBuffer>& buffer);

} // namespace pixedit

#endif /* PIXEDIT_SRC_EDITOR_APP_VIEW_SETTINGS_INCLUDED */
