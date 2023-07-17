#ifndef PIXEDIT_SRC_ID_INCLUDED
#define PIXEDIT_SRC_ID_INCLUDED

#include <string>
#include <string_view>
#include <utility>

namespace pixedit {

using Id = std::string_view;
using NameId = std::string;

struct IdRef
{
  size_t value;

  constexpr IdRef() = default;
  IdRef(Id id);
  IdRef(NameId id)
    : IdRef(Id(id)){};
};

constexpr bool
operator==(IdRef lhs, IdRef rhs)
{
  return lhs.value == rhs.value;
}

} // namespace pixedit

namespace std {
template<>
struct hash<pixedit::IdRef>
{
  constexpr std::size_t operator()(pixedit::IdRef idRef) const noexcept
  {
    return idRef.value;
  }
};

} // namespace std

#endif /* PIXEDIT_SRC_ID_INCLUDED */
