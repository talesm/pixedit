#ifndef PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED
#define PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED

#include <list>
#include <string>
#include <SDL.h>
#include "TempSurface.hpp"
#include "utils/Surface.hpp"

namespace pixedit {
/**
 * The editing target
 */
class PictureBuffer
{
private:
  std::string filename;
  Surface surface;
  std::list<TempSurface> history;
  std::list<TempSurface>::iterator historyPoint = history.end();
  std::list<TempSurface>::iterator lastSave = history.end();
  Surface selectionSurface;
  SDL_Rect selectionRect{0, 0, 10, 10};

public:
  PictureBuffer() = default;
  PictureBuffer(std::string filename);
  PictureBuffer(std::string filename, Surface surface)
    : filename(std::move(filename))
    , surface(surface)
  {
    if (surface) { makeSnapshot(); }
  }

  /// @brief True if this needs saving
  bool isDirty() const { return lastSave != historyPoint; }

  bool save(bool force = false);

  bool saveAs(const std::string& filename);

  bool saveCopy(const std::string& filename);

  void makeSnapshot();

  void refresh();

  bool undo();

  bool redo();

  constexpr const std::string& getFilename() const { return filename; }

  Surface getSurface() const { return surface; }

  int getW() const { return surface.getW(); }
  int getH() const { return surface.getH(); }
  SDL_Point getSize() const { return {getW(), getH()}; }

  constexpr SDL_Rect& getSelectionRect() { return selectionRect; }
  constexpr const SDL_Rect& getSelectionRect() const { return selectionRect; }
  constexpr void setSelectionRect(SDL_Rect rect) { selectionRect = rect; }

  constexpr const Surface& getSelectionSurface() const
  {
    return selectionSurface;
  }
  bool hasSelection() const { return selectionSurface; }
  void clearSelection() { selectionSurface.reset(); }
  void setSelection(Surface surface, SDL_Rect rect)
  {
    selectionSurface = std::move(surface);
    selectionRect = rect;
  }

  void persistSelection()
  {
    surface.blitScaled(selectionSurface, selectionRect);
    clearSelection();
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED */
