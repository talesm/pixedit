#ifndef PIXEDIT_SRC_UTILS_TEMP_BUFFER_INCLUDED
#define PIXEDIT_SRC_UTILS_TEMP_BUFFER_INCLUDED

#include <string>
#include <string_view>
#include <SDL.h>

namespace pixedit {

std::string
makeTempFilename(std::string_view prefix, std::string_view suffix);

/// @brief A class to hold temporarily a surface
class TempSurface
{
private:
  std::string filename;

public:
  TempSurface(SDL_Surface* surface);
  TempSurface(const TempSurface&) = delete;
  TempSurface(TempSurface&& rhs) { std::swap(filename, rhs.filename); }
  ~TempSurface() { reset(); }
  TempSurface& operator=(TempSurface rhs)
  {
    std::swap(filename, rhs.filename);
    return *this;
  }

  SDL_Surface* recover() const;
  void reset();

  const std::string& getFilename() const { return filename; }
};
} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_TEMP_BUFFER_INCLUDED */
