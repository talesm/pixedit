#ifndef PIXEDIT_SRC_LOADERS_INCLUDED
#define PIXEDIT_SRC_LOADERS_INCLUDED

#include <memory>
#include <string>
#include "Id.hpp"

namespace pixedit {

// Forward decl
class Surface;
class PictureBuffer;

namespace loaders {

constexpr Id PIX{"pix"};
constexpr Id SDL2_IMAGE{"sdl2_image"};
constexpr Id TEXT{"text"};

} // namespace loaders

std::unique_ptr<PictureBuffer>
loadBuffer(const std::string& filename, Id loader = {});

Surface
loadSurface(const std::string& filename, Id loader = {});

} // namespace pixedit

#endif /* PIXEDIT_SRC_LOADERS_INCLUDED */
