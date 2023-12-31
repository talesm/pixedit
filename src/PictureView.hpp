#ifndef PIXEDIT_SRC_PICTURE_VIEW_INCLUDED
#define PIXEDIT_SRC_PICTURE_VIEW_INCLUDED

#include <memory>
#include <optional>
#include <SDL.h>
#include "Canvas.hpp"
#include "MouseState.hpp"
#include "PictureBuffer.hpp"
#include "ToolDescription.hpp"

namespace pixedit {

class PictureView
{
  SDL_Rect viewport;
  std::shared_ptr<PictureBuffer> buffer, newBuffer;
  MouseState oldState{};
  SDL_Texture* preview = nullptr;
  Surface scratch;

  bool scratchEnabled = false;
  bool changed = false;
  bool editing = false;
  bool transparent = true;

  Id toolId = "Move";
  std::optional<IdRef> nextToolId;
  Tool tool = nullptr;

  bool movingMode = false;
  SDL_Color checkerColors[2] = {{200, 200, 200, 255}, {150, 150, 150, 255}};
  int checkerSize = 16;
  bool grid = true;

public:
  SDL_FPoint offset{0};
  float scale{1.f};
  MouseState state{};
  Canvas canvas;
  bool fillSelectedOut = false;

  PictureView(const SDL_Rect& viewport = {0})
    : viewport(viewport)
  {
  }

  void previewEdit() { changed = true; }

  void beginEdit()
  {
    if (!buffer) return;
    editing = true;
  }

  constexpr bool isEditing() const { return editing; }

  void endEdit()
  {
    enableScratch(false);
    if (!buffer || !editing) return;
    buffer->makeSnapshot();
    previewEdit();
    editing = false;
  }

  void cancelEdit()
  {
    enableScratch(false);
    if (!buffer || !editing) return;
    editing = false;
    oldState.left = oldState.right = (oldState.left || oldState.right);
    buffer->refresh();
    previewEdit();
  }

  bool undo()
  {
    if (!buffer) return false;
    cancelEdit();
    changed = true;
    return buffer->undo();
  }

  bool redo()
  {
    if (!buffer) return false;
    cancelEdit();
    changed = true;
    return buffer->redo();
  }

  constexpr bool isScratchEnabled() const { return scratchEnabled; }

  void enableScratch(bool enable = true);

  void update(SDL_Renderer* renderer);

  void render(SDL_Renderer* renderer) const;

  float effectiveScale() const;

  SDL_FPoint effectiveSize() const;

  SDL_FPoint effectiveOffset() const;

  float setScale(float value)
  {
    scale = value;
    return scale = effectiveScale();
  }

  Id getToolId() { return toolId; }

  void setToolId(IdRef id) { nextToolId = id; }

  constexpr bool isTransparent() { return transparent; }

  void setTransparent(bool value);

  SDL_Point effectivePos() const;

  constexpr const SDL_Rect& getViewport() { return viewport; }

  void setViewport(const SDL_Rect& value) { viewport = value; }

  const std::shared_ptr<PictureBuffer>& getBuffer() const { return buffer; }

  void setBuffer(std::shared_ptr<PictureBuffer> value) { newBuffer = value; }

  constexpr const MouseState& getState() const { return state; }
  constexpr const MouseState& getOldState() const { return oldState; }

  void setSelection(Surface surface);
  void persistSelection();

  void pickColorUnderMouse();

  constexpr bool isGridEnabled() const { return grid; }
  constexpr void enableGrid(bool value) { grid = value; }

private:
  void updatePreview(SDL_Renderer* renderer);
};

} // namespace pixedit

#endif /* PIXEDIT_SRC_PICTURE_VIEW_INCLUDED */
