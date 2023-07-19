#include "PictureManager.hpp"

namespace pixedit {

void
PictureManager::swapView(PictureView* view)
{
  if (view != lastView) {
    if (lastView) {
      toolId = lastView->getToolId();
      lastView->cancelEdit();
      brush = lastView->canvas.getBrush();
      colorA = lastView->canvas.getColorA();
      colorB = lastView->canvas.getColorB();
      transparent = lastView->isTransparent();
    }
    if (view) {
      view->canvas | brush | ColorA{colorA} | ColorB{colorB};
      view->setTransparent(transparent);
    }
    lastView = view;
  }
}

void
PictureManager::update(PictureView* view,
                       SDL_Renderer* renderer,
                       const Rect& viewport)
{
  swapView(view);
  if (!view) return;
  if (view->getToolId() != toolId) { view->setToolId(toolId); }
  view->setViewport(viewport);
  view->update(renderer);
}

void
PictureManager::render(PictureView* view,
                       SDL_Renderer* renderer,
                       const Rect& viewport) const
{
  if (!view) return;
  view->setViewport(viewport);
  view->render(renderer);
}

void
PictureManager::setBrush(Brush value)
{
  brush = value;
  if (lastView) { lastView->canvas | brush; }
}

void
PictureManager::setColorA(Color value)
{
  colorA = value;
  if (lastView) { lastView->canvas | ColorA{colorA}; }
}

void
PictureManager::setColorB(Color value)
{
  colorB = value;
  if (lastView) { lastView->canvas | ColorA{colorB}; }
}

void
PictureManager::setTransparent(bool value)
{
  transparent = value;
  if (lastView) { lastView->setTransparent(transparent); }
}

} // namespace pixedit
