#ifndef PIXEDIT_SRC_UTILS_DUMP_SURFACE_INCLUDED
#define PIXEDIT_SRC_UTILS_DUMP_SURFACE_INCLUDED

#include <iostream>
#include "Surface.hpp"
#include "pixel.hpp"

namespace pixedit::dump {

inline void
format(std::ostream& out, const Surface& s)
{
  SDL_assert(s);
  int bitsPerPixel = s.GetFormat().GetBitsPerPixel();
  out << "FORMAT " << s.GetWidth() << ' ' << s.GetHeight() << ' '
      << bitsPerPixel << '\n';
}

inline void
data(std::ostream& out,
     const Surface& s,
     int charsPerLine = 0,
     bool newlineAfterRow = false)
{
  SDL_assert(s);
  int bitsPerPixel = s.GetFormat().GetBitsPerPixel();
  int bytesPerPixel = bitsPerPixel / 8 + (bitsPerPixel % 8 != 0);
  int charsPerPixel = bytesPerPixel * 2;

  out << "DATA:\n";
  out << std::hex;
  int charCount = 0;
  for (int y = 0; y < s.GetHeight(); y++) {
    for (int x = 0; x < s.GetWidth(); x++) {
      if (charsPerLine && charCount + charsPerPixel > charsPerLine) {
        charCount = 0;
        out << '\n';
      } else if (charCount) {
        out << ' ';
        charCount++;
      }
      out.width(charsPerPixel);
      out.fill('0');
      out << getPixelAt(s, x, y);
      charCount += charsPerPixel;
    }
    if (newlineAfterRow) {
      charCount = 0;
      out << '\n';
    }
  }
}

inline void
surface(std::ostream& out,
        const Surface& s,
        int charsPerLine = 0,
        bool newlineAfterRow = false)
{
  format(out, s);
  data(out, s, charsPerLine, newlineAfterRow);
}

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_DUMP_SURFACE_INCLUDED */
