#include "Id.hpp"
#include <bit>

namespace pixedit {

IdRef::IdRef(Id id)
  : value(0)
{
  size_t params[] = {5, 13, 37, 69, 148};
  size_t offset = 0;
  size_t c = 0;
  for (size_t i = 0; i < sizeof(value); i++) { value = (value << 8) | 0x55; }
  for (unsigned char ch : id) {
    offset = (offset + ch) % 7 + 1;
    value = std::rotl(value, offset) ^ ch ^ params[c];
    c = (c + 1) % 5;
  }
}

} // namespace pixedit
