#ifndef PIXEDIT_SRC_SAVERS_INCLUDED
#define PIXEDIT_SRC_SAVERS_INCLUDED

#include <string>
#include "Id.hpp"
#include "PictureFile.hpp"

namespace pixedit {

namespace savers {

constexpr Id PIX{"pix"};
constexpr Id SDL2_IMAGE_PNG{"sdl2_image.png"};
constexpr Id SDL2_IMAGE_JPEG{"sdl2_image.jpg"};
constexpr Id SDL2_BMP{"sdl2.bmp"};
constexpr Id TEXT{"text"};

Saver
get(Id saver);

} // namespace savers
// Forward decl
class Surface;
class PictureBuffer;

bool
saveBuffer(const PictureBuffer& buffer,
           const std::string& filename,
           Id saver = {});

Id
saverForFile(const std::string& filename);

bool
saveSurface(const Surface& surface, const std::string& filename, Id saver = {});

} // namespace pixedit

#endif /* PIXEDIT_SRC_SAVERS_INCLUDED */
