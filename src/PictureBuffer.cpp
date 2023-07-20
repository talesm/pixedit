#include "PictureBuffer.hpp"
#include "loaders.hpp"

namespace pixedit {

namespace defaults {
extern const unsigned HISTORY_MAX;
} // namespace defaults

std::unique_ptr<PictureBuffer>
PictureBuffer::load(const std::string& filename)
{
  return loadBuffer(filename);
}

bool
PictureBuffer::save(bool force)
{
  if (!force && lastSave == historyPoint) return false;
  if (!saveCopy(filename)) return false;
  lastSave = historyPoint;
  return true;
}
bool
PictureBuffer::saveAs(const std::string& filename)
{
  std::string backup = std::move(this->filename);
  this->filename = filename;
  if (save(true)) return true;
  std::swap(this->filename, backup);
  return false;
}
bool
PictureBuffer::saveCopy(const std::string& filename)
{
  return surface.save(filename);
}

void
PictureBuffer::makeSnapshot()
{
  if (!surface) return;
  if (selectionSurface) clearSelection();
  if (historyPoint != history.end()) {
    history.erase(++historyPoint, history.end());
  }
  historyPoint = history.emplace(history.end(), surface);
  if (history.size() > defaults::HISTORY_MAX) history.pop_front();
}

void
PictureBuffer::refresh()
{
  if (!surface || history.empty() || historyPoint == history.end()) return;
  if (selectionSurface) clearSelection();
  surface = historyPoint->recover();
}

bool
PictureBuffer::undo()
{
  if (!surface || history.empty() || historyPoint == history.begin()) {
    return false;
  }
  --historyPoint;
  refresh();
  return true;
}

bool
PictureBuffer::redo()
{
  if (!surface || history.empty()) { return false; }
  ++historyPoint;
  if (historyPoint == history.end()) {
    --historyPoint;
    return false;
  }
  refresh();
  return false;
}

void
PictureBuffer::persistSelection()
{
  if (selectionMask) {
    selectionMask.setColorIndex(0, {0, 0, 0, 0});
    selectionMask.setColorKey(1);
    selectionSurface.blit(selectionMask);
    if (!selectionSurface.getColorKey() &&
        selectionSurface.getBlendMode() != SDL_BLENDMODE_BLEND) {
      selectionMask.setColorKey(0);
      selectionMask.setColorIndex(1, {0, 0, 0, 255});
      surface.blitScaled(selectionMask, selectionRect);
      selectionSurface.setBlendMode(SDL_BLENDMODE_ADD);
    }
  }
  surface.blitScaled(selectionSurface, selectionRect);
  clearSelection();
}

} // namespace pixedit
