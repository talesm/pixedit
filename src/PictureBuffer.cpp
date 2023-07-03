#include "PictureBuffer.hpp"
#include <SDL_image.h>

namespace pixedit {

PictureBuffer::PictureBuffer(std::string filename)
  : PictureBuffer(std::move(filename), IMG_Load(filename.c_str()), true)
{
}

} // namespace pixedit
