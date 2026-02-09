#ifndef PIXEDIT_SRC_UTILS_TEMP_BUFFER_INCLUDED
#define PIXEDIT_SRC_UTILS_TEMP_BUFFER_INCLUDED

#include <string>
#include <string_view>
#include "Surface.hpp"

namespace pixedit {

std::string
makeTempFilename(std::string_view prefix, std::string_view suffix);

/// @brief A class to hold temporarily a surface
class TempSurface
{

public:
  constexpr TempSurface() = default;
  TempSurface(const Surface& surface);
  TempSurface(const Surface& surface, std::string filename);
  TempSurface(const TempSurface&) = delete;
  TempSurface(TempSurface&& rhs) noexcept
  { std::swap(filename_, rhs.filename_); }
  ~TempSurface() { reset(); }
  TempSurface& operator=(TempSurface rhs)
  {
    std::swap(filename_, rhs.filename_);
    return *this;
  }

  Surface recover() const;
  void reset();

  const std::string& filename() const { return filename_; }

private:
  std::string filename_;
};
} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_TEMP_BUFFER_INCLUDED */
