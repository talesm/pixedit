#include "PictureBuffer.hpp"
#include "Color.hpp"
#include "TempSurface.hpp"
#include "loaders.hpp"
#include "savers.hpp"

namespace pixedit {

namespace defaults {
extern const unsigned HISTORY_MAX;
} // namespace defaults

PictureBuffer::PictureBuffer(PictureFile file, Surface surface_, bool dirty)
  : file(std::move(file))
  , surface(std::move(surface_))
  , document(PixDocument::convert(makeTempFilename("temp_", ".db"), surface))
  , currentVersion(document.getLatestVersion())
  , savedVersion(!dirty ? currentVersion : 0)
{
}

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
{ return loadBuffer(filename); }

bool
PictureBuffer::save(bool force)
{
  if (!force && !isDirty()) return false;
  if (!file.save(*this)) return false;
  savedVersion = currentVersion;
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
{ return saveBuffer(*this, filename); }

void
PictureBuffer::makeSnapshot()
{
  if (!surface) return;
  if (selectionSurface) clearSelection();
  lastVersion = currentVersion = document.newVersion(currentVersion);
  document.putSurface(1, surface);
}

void
PictureBuffer::refresh()
{
  if (!surface) return;
  if (selectionSurface) clearSelection();
  document.getSurface(currentVersion, 1, &surface);
}

bool
PictureBuffer::undo()
{
  if (!surface || currentVersion <= 1) return false;
  --currentVersion;
  refresh();
  return true;
}

bool
PictureBuffer::redo()
{
  if (!surface || currentVersion >= lastVersion) { return false; }
  ++currentVersion;
  refresh();
  return true;
}

void
PictureBuffer::persistSelection()
{
  if (selectionMask) {
    auto palette = selectionMask.GetPalette();
    Color newColor = {{0, 0, 0, 0}};
    palette.SetColors({&newColor, 1}, 0);
    selectionMask.SetColorKey(1);
    selectionSurface.Blit(selectionMask, {}, {});
    if (!selectionSurface.GetColorKey() &&
        selectionSurface.GetBlendMode() != SDL::BLENDMODE_BLEND) {
      selectionMask.SetColorKey(0);
      palette.SetColors({&newColor, 1}, 1);
      surface.BlitScaled(
        selectionMask, {}, selectionRect, SDL::SCALEMODE_NEAREST);
      selectionSurface.SetBlendMode(SDL::BLENDMODE_ADD);
    }
  }
  surface.BlitScaled(
    selectionSurface, {}, selectionRect, SDL::SCALEMODE_NEAREST);
  clearSelection();
}

} // namespace pixedit
