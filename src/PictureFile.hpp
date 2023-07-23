#ifndef PIXEDIT_SRC_PICTURE_FILE_INCLUDED
#define PIXEDIT_SRC_PICTURE_FILE_INCLUDED

#include <memory>
#include <string>

namespace pixedit {

// Forward decl
class PictureBuffer;

using Loader = std::unique_ptr<PictureBuffer> (*)(const std::string& filename);
using Saver = bool (*)(const PictureBuffer* buffer,
                       const std::string& filename);

struct PictureFile
{
  std::string name;
  Loader loader;
  Saver saver;

  PictureFile(std::string name, Loader loader, Saver saver)
    : name(std::move(name))
    , loader(loader)
    , saver(saver){};

  PictureFile() = default;

  explicit PictureFile(std::string name)
    : PictureFile(std::move(name), nullptr, nullptr){};

  bool load(PictureBuffer& buffer);

  bool save(const PictureBuffer& buffer);

  operator bool() const { return !name.empty(); }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_FILE_INCLUDED */
