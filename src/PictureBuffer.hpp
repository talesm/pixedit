#ifndef PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED
#define PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED

#include <list>
#include <memory>
#include <string>
#include <SDL.h>
#include "PictureFile.hpp"
#include "Surface.hpp"
#include "utils/TempSurface.hpp"

namespace pixedit {
/**
 * The editing target
 */
class PictureBuffer
{
private:
  PictureFile file;
  Surface surface;
  std::list<TempSurface> history;
  std::list<TempSurface>::iterator historyPoint = history.end();
  std::list<TempSurface>::iterator lastSave = history.end();
  Surface selectionSurface;
  Surface selectionMask;
  SDL_Rect selectionRect{0, 0, 10, 10};

public:
  PictureBuffer() = default;
  PictureBuffer(std::string filename, Surface surface, bool dirty = false)
    : PictureBuffer(PictureFile{filename}, std::move(surface), dirty){};

  PictureBuffer(PictureFile file, Surface surface, bool dirty = false)
    : file(std::move(file))
    , surface(surface)
  {
    if (surface) {
      makeSnapshot();
      if (!dirty) { lastSave = history.begin(); }
    }
  }

  static std::unique_ptr<PictureBuffer> load(const std::string& filename);

  /// @brief True if this needs saving
  bool isDirty() const { return lastSave != historyPoint; }

  bool save(bool force = false);

  bool saveAs(const std::string& filename);

  bool saveCopy(const std::string& filename);

  void makeSnapshot();

  void refresh();

  bool undo();

  bool redo();

  constexpr const std::string& getFilename() const { return file.name; }

  constexpr const PictureFile& getFile() const { return file; }

  Surface getSurface() const { return surface; }

  void setSurface(Surface value) { surface = value; }

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
  constexpr const Surface& getSelectionMask() const { return selectionMask; }

  bool hasSelection() const { return selectionSurface; }
  void clearSelection() { selectionSurface.reset(); }
  void setSelection(Surface surface, SDL_Rect rect)
  {
    selectionSurface = std::move(surface);
    selectionRect = rect;
    selectionMask.reset();
  }
  void setSelection(Surface surface, SDL_Rect rect, Surface mask)
  {
    selectionSurface = std::move(surface);
    selectionRect = rect;
    selectionMask = std::move(mask);
  }

  void persistSelection();
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED */
