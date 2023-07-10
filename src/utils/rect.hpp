#ifndef PIXEDIT_SRC_UTILS_RECT_INCLUDED
#define PIXEDIT_SRC_UTILS_RECT_INCLUDED

#include <SDL.h>

namespace pixedit {

struct Point : SDL_Point
{
  constexpr Point()
    : SDL_Point{0} {};
  constexpr Point(const SDL_Point& p)
    : SDL_Point{p} {};
  constexpr Point(int x, int y)
    : SDL_Point{x, y} {};
  constexpr explicit Point(const SDL_FPoint& p)
    : SDL_Point{int(p.x), int(p.y)} {};

  constexpr Point operator-() const { return {-x, -y}; }
  constexpr Point& operator+=(const Point& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }
  constexpr Point& operator-=(const Point& rhs) { return operator+=(-rhs); }
};
constexpr Point
operator+(Point lhs, const Point& rhs)
{
  return lhs += rhs;
}
constexpr Point
operator-(Point lhs, const Point& rhs)
{
  return lhs -= rhs;
}

struct FPoint : SDL_FPoint
{
  constexpr FPoint()
    : SDL_FPoint{0} {};
  constexpr FPoint(const SDL_FPoint& p)
    : SDL_FPoint{p} {};
  constexpr FPoint(float x, float y)
    : SDL_FPoint{x, y} {};
  constexpr FPoint(const SDL_Point& p)
    : FPoint(float(p.x), float(p.y)){};

  constexpr FPoint operator-() const { return {-x, -y}; }
  constexpr FPoint& operator+=(const FPoint& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }
  constexpr FPoint& operator-=(const FPoint& rhs) { return operator+=(-rhs); }
};
constexpr FPoint
operator+(FPoint lhs, const FPoint& rhs)
{
  return lhs += rhs;
}
constexpr FPoint
operator-(FPoint lhs, const FPoint& rhs)
{
  return lhs -= rhs;
}

constexpr SDL_Rect
intersectFromOrigin(SDL_Rect rect, const SDL_Point ceil)
{
  if (rect.x < 0) {
    rect.w += rect.x;
    rect.x = 0;
  }
  if (rect.y < 0) {
    rect.h += rect.y;
    rect.y = 0;
  }
  if (rect.x + rect.w > ceil.x) { rect.w = ceil.x - rect.x; }
  if (rect.y + rect.h > ceil.y) { rect.h = ceil.y - rect.y; }
  return rect;
}

struct Rect : SDL_Rect
{
  constexpr Rect()
    : SDL_Rect{0} {};
  constexpr Rect(const SDL_Rect& p)
    : SDL_Rect{p} {};
  constexpr Rect(int x, int y, int w, int h)
    : SDL_Rect{x, y, w, h} {};
  constexpr explicit Rect(const SDL_FRect& p)
    : SDL_Rect{int(p.x), int(p.y)} {};

  constexpr static Rect fromPoints(const Point& a, const Point& b)
  {
    SDL_Rect rect;
    if (a.x < b.x) {
      rect.x = a.x;
      rect.w = b.x - a.x + 1;
    } else {
      rect.x = b.x;
      rect.w = a.x - b.x + 1;
    }
    if (a.y < b.y) {
      rect.y = a.y;
      rect.h = b.y - a.y + 1;
    } else {
      rect.y = b.y;
      rect.h = a.y - b.y + 1;
    }
    return rect;
  }
  constexpr static Rect fromPoints(int x1, int y1, int x2, int y2)
  {
    return fromPoints({x1, y1}, {x2, y2});
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_UTILS_RECT_INCLUDED */
