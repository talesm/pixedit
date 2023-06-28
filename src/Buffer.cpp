#include "Buffer.hpp"
#include <SDL_image.h>

namespace pixedit {

Buffer::Buffer(std::string filename)
  : Buffer(std::move(filename), IMG_Load(filename.c_str()), true)
{
}

} // namespace pixedit
