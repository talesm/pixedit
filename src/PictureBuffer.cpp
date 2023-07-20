#include "PictureBuffer.hpp"
#include "loaders.hpp"
#include "savers.hpp"

namespace pixedit {

namespace defaults {
extern const unsigned HISTORY_MAX;
} // namespace defaults

bool
PictureFile::load(PictureBuffer& buffer)
{
  auto b = (loader) ? loader(name) : loadBuffer(name);
  if (!b) return false;
  loader = b->getFile().loader;
  buffer.setSurface(b->getSurface());
  return true;
}

bool
PictureFile::save(const PictureBuffer& buffer)
{
  if (!saver) {
    saver = savers::get(saverForFile(name));
    if (!saver) return {};
  }
  return saver(&buffer, name);
}

std::unique_ptr<PictureBuffer>
PictureBuffer::load(const std::string& filename)
{
  return loadBuffer(filename);
}

bool
PictureBuffer::save(bool force)
{
  if (!force && lastSave == historyPoint) return false;
  if (!file.save(*this)) return false;
  lastSave = historyPoint;
  return true;
}
bool
PictureBuffer::saveAs(const std::string& filename)
{
  auto backup = std::move(file);
  file = PictureFile{filename};
  if (save(true)) {
    if (file.saver == backup.saver) { file.loader = backup.loader; }
    return true;
  }
  std::swap(file, backup);
  return false;
}
bool
PictureBuffer::saveCopy(const std::string& filename)
{
  return saveBuffer(*this, filename);
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
