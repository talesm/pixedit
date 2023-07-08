#ifndef PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED
#define PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED

#include <list>
#include <string>
#include <SDL.h>
#include "TempSurface.hpp"

namespace pixedit {
/**
 * The editing target
 */
class PictureBuffer
{
private:
  std::string filename;
  SDL_Surface* surface = nullptr;
  std::list<TempSurface> history;
  std::list<TempSurface>::iterator historyPoint = history.end();
  std::list<TempSurface>::iterator lastSave = history.end();
  SDL_Surface* selectionSurface = nullptr;
  SDL_Rect selectionRect{0, 0, 10, 10};

public:
  PictureBuffer() = default;
  PictureBuffer(std::string filename);
  PictureBuffer(std::string filename, SDL_Surface* surface, bool owner = false)
    : filename(std::move(filename))
    , surface(surface)
  {
    if (!owner) surface->refcount++;
    if (surface) { makeSnapshot(); }
  }

  ~PictureBuffer() { SDL_FreeSurface(surface); }
  PictureBuffer(const PictureBuffer&) = delete;
  PictureBuffer(PictureBuffer&& rhs)
    : filename(std::move(rhs.filename))
    , surface(rhs.surface)
  {
    rhs.surface = nullptr;
  }
  PictureBuffer& operator=(PictureBuffer rhs)
  {
    std::swap(filename, rhs.filename);
    std::swap(surface, rhs.surface);
    return *this;
  }

  /// @brief True if this needs saving
  bool isDirty() const { return lastSave == historyPoint; }

  bool save(bool force = false);

  bool saveAs(const std::string& filename);

  bool saveCopy(const std::string& filename);

  void makeSnapshot();

  void refresh();

  bool undo();

  bool redo();

  constexpr const std::string& getFilename() const { return filename; }

  constexpr SDL_Surface* getSurface() const { return surface; }

  constexpr int getW() const { return surface ? surface->w : 0; }
  constexpr int getH() const { return surface ? surface->h : 0; }
  constexpr SDL_Point getSize() const { return {getW(), getH()}; }

  constexpr SDL_Rect& getSelectionRect() { return selectionRect; }
  constexpr const SDL_Rect& getSelectionRect() const { return selectionRect; }
  constexpr void setSelectionRect(SDL_Rect rect) { selectionRect = rect; }

  constexpr auto getSelectionSurface() const { return selectionSurface; }
  constexpr bool hasSelection() const { return selectionSurface; }
  void clearSelection()
  {
    SDL_FreeSurface(selectionSurface);
    selectionSurface = nullptr;
  }
  void setSelection(SDL_Surface* surface, SDL_Rect rect)
  {
    SDL_FreeSurface(selectionSurface);
    selectionSurface = surface;
    selectionRect = rect;
  }

  void persistSelection()
  {
    SDL_BlitSurface(selectionSurface, nullptr, surface, &selectionRect);
    clearSelection();
  }
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_BUFFER_INCLUDED */
