#ifndef PIXEDIT_SRC_PICTURE_MANAGER_INCLUDED
#define PIXEDIT_SRC_PICTURE_MANAGER_INCLUDED

#include "Brush.hpp"
#include "PictureView.hpp"
#include "tools.hpp"
#include "utils/Color.hpp"

namespace pixedit {

class PictureManager
{
private:
  PictureView* lastView = nullptr;
  Id toolId = tools::FREE_HAND;
  Brush brush{};
  Color colorA{0, 0, 0, 255}, colorB{255, 255, 255, 255};
  bool transparent = true;

public:
  void update(PictureView* view, SDL_Renderer* renderer, const Rect& viewport);
  void render(PictureView* view,
              SDL_Renderer* renderer,
              const Rect& viewport) const;

  void swapView(PictureView* view);

  constexpr Id getToolId() const { return toolId; }
  constexpr void setToolId(Id id) { toolId = id; }

  Brush getBrush() const { return brush; }
  void setBrush(Brush value);

  Color getColorA() const { return colorA; }
  void setColorA(Color value);

  Color getColorB() const { return colorB; }
  void setColorB(Color value);

  bool isTransparent() const { return transparent; }
  void setTransparent(bool value);

  void swapColors()
  {
    auto colorA = getColorA();
    auto colorB = getColorB();
    setColorA(colorB);
    setColorB(colorA);
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_MANAGER_INCLUDED */
