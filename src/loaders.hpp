#ifndef PIXEDIT_SRC_LOADERS_INCLUDED
#define PIXEDIT_SRC_LOADERS_INCLUDED

#include <memory>
#include <string>
#include "Id.hpp"
#include "Surface.hpp"

namespace pixedit {

// Forward decl
class PictureBuffer;

namespace loaders {

constexpr Id PIX{"pix"};
constexpr Id SDL_IMAGE{"sdl_image"};
constexpr Id TEXT{"text"};

} // namespace loaders

std::unique_ptr<PictureBuffer>
loadBuffer(const std::string& filename, Id loader = {});

Surface
loadSurface(const std::string& filename, Id loader = {});

} // namespace pixedit

#endif /* PIXEDIT_SRC_LOADERS_INCLUDED */
