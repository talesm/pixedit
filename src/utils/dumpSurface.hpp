#ifndef PIXEDIT_SRC_UTILS_DUMP_SURFACE_INCLUDED
#define PIXEDIT_SRC_UTILS_DUMP_SURFACE_INCLUDED

#include <iostream>
#include "Surface.hpp"

namespace pixedit::dump {

inline void
format(std::ostream& out, const Surface& s)
{
  SDL_assert(s);
  int bitsPerPixel = s.getFormat()->BitsPerPixel;
  out << "FORMAT " << s.getW() << ' ' << s.getH() << ' ' << bitsPerPixel
      << '\n';
}

inline void
data(std::ostream& out,
     const Surface& s,
     int charsPerLine = 0,
     bool newlineAfterRow = false)
{
  SDL_assert(s);
  int bitsPerPixel = s.getFormat()->BitsPerPixel;
  int bytesPerPixel = bitsPerPixel / 8 + (bitsPerPixel % 8 != 0);
  int charsPerPixel = bytesPerPixel * 2;

  out << "DATA:\n";
  out << std::hex;
  int charCount = 0;
  for (int y = 0; y < s.getH(); y++) {
    for (int x = 0; x < s.getW(); x++) {
      if (charsPerLine && charCount + charsPerPixel > charsPerLine) {
        charCount = 0;
        out << '\n';
      } else if (charCount) {
        out << ' ';
        charCount++;
      }
      out.width(charsPerPixel);
      out.fill('0');
      out << s.getPixel(x, y);
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
