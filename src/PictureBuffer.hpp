#ifndef PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED
#define PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED

#include <list>
#include <memory>
#include <string>
#include <utility>
#include <SDL3/SDL.h>
#include "PictureFile.hpp"
#include "Surface.hpp"
#include "utils/TempSurface.hpp"
#include "utils/rect.hpp"

namespace pixedit {
/**
 * The editing target
 */
class PictureBuffer
{
  PictureFile file;
  Surface surface;
  std::list<TempSurface> history;
  std::list<TempSurface>::iterator historyPoint = history.end();
  std::list<TempSurface>::iterator lastSave = history.end();
  Surface selectionSurface;
  Surface selectionMask;
  Rect selectionRect{0, 0, 10, 10};

public:
  PictureBuffer() = default;
  PictureBuffer(std::string filename, Surface s, bool dirty = false)
    : PictureBuffer(PictureFile{std::move(filename)}, std::move(s), dirty) {};

  PictureBuffer(PictureFile file, Surface surface_, bool dirty = false)
    : file(std::move(file))
    , surface(std::move(surface_))
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

  void setSurface(Surface value) { surface = std::move(value); }

  int GetWidth() const { return surface.GetWidth(); }
  int GetHeight() const { return surface.GetHeight(); }
  Point getSize() const { return surface.GetSize(); }

  constexpr SDL_Rect& getSelectionRect() { return selectionRect; }
  constexpr const SDL_Rect& getSelectionRect() const { return selectionRect; }
  constexpr void setSelectionRect(SDL_Rect rect) { selectionRect = rect; }

  constexpr const Surface& getSelectionSurface() const
  { return selectionSurface; }
  constexpr const Surface& getSelectionMask() const { return selectionMask; }

  bool hasSelection() const { return selectionSurface != nullptr; }
  void clearSelection() { selectionSurface.Destroy(); }
  void setSelection(Surface s, SDL_Rect rect)
  {
    selectionSurface = std::move(s);
    selectionRect = rect;
    selectionMask.Destroy();
  }
  void setSelection(Surface s, SDL_Rect rect, Surface mask)
  {
    selectionSurface = std::move(s);
    selectionRect = rect;
    selectionMask = std::move(mask);
  }

  void persistSelection();
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED */
