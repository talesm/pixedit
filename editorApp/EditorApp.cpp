#include "EditorApp.hpp"
#include <sstream>
#include <stdexcept>
#include <imgui.h>
#include "Action.hpp"
#include "Clipboard.hpp"
#include "FileDialogTinyfd.hpp"
#include "ImGuiComponent.hpp"
#include "PictureView.hpp"
#include "PluginManager.hpp"
#include "ViewSettings.hpp"
#include "actions.hpp"
#include "imgui/AboutDialog.hpp"
#include "imgui/AuxWindowManager.hpp"
#include "imgui/BufferSelectionWindow.hpp"
#include "imgui/ConfirmExitDialog.hpp"
#include "imgui/MainMenu.hpp"
#include "imgui/NewFileDialog.hpp"
#include "imgui/PictureOptionsWindow.hpp"
#include "imgui/PictureWindow.hpp"
#include "shortcutPlugin.hpp"

namespace pixedit {

namespace defaults {
extern const bool ASK_SAVE_ON_CLOSE;
} // namespace defaults

struct EditorState
{
  SDL::Window window;
  SDL::Renderer renderer;
  PluginManager plugins;
  ImGuiComponent ui{window.get(), renderer.get()};

  PictureManager picture;
  Rect pictureViewport;
  PictureView view;
  bool exited = false;
  bool maximizeView = false;

  std::vector<std::shared_ptr<PictureBuffer>> buffers;
  std::map<std::shared_ptr<PictureBuffer>, ViewSettings> viewSettings;
  int bufferIndex = -1;

  ActionManager actions;
  ShortcutManager shortcuts;
  AuxWindowManager auxWindows;

  bool exiting = false;
  bool focusBufferNextFrame = true;
  std::string requestModal;
};

static EditorState* ctx = nullptr;

struct EditorAppImpl final
  : EditorApp
  , EditorState
{

  EditorAppImpl(SDL::Window window, SDL::Renderer, const Rect& pictureViewport);

  ~EditorAppImpl() override
  {
    ctx = nullptr;
    SDL::Quit();
  }

  void update();

  void setupInitialBuffers(const EditorInitSettings& settings);

  SDL::AppResult Iterate() override;

  SDL::AppResult Event(const SDL::Event& e) override;

  void close(bool force);

  void setupActions();
};

void
EditorAppImpl::update()
{
  if (maximizeView) {
    if (!ImGui::GetIO().WantCaptureMouse &&
        SDL_GetMouseFocus() == window.get()) {
      auto buttonState = SDL_GetMouseState(&view.state.x, &view.state.y);
      view.state.left = buttonState & SDL_BUTTON_LMASK;
      view.state.middle = buttonState & SDL_BUTTON_MMASK;
      view.state.right = buttonState & SDL_BUTTON_RMASK;
    };
    picture.update(&view, renderer.get(), pictureViewport);
  }
  ui.update();
  showMainMenuBar(currentView(), &maximizeView);
  auxWindows.showAll();

  if (!requestModal.empty()) {
    ImGui::OpenPopup(requestModal.c_str());
    requestModal.clear();
  }
  showNewFileDialog();
  showConfirmExitDialog(&exiting);
  showAboutDialog();

  if (!maximizeView) {
    for (auto& buffer : buffers) {
      if (focusBufferNextFrame && buffer == currentBuffer()) {
        focusBufferNextFrame = false;
        ImGui::SetNextWindowFocus();
      }
      showPictureWindow(renderer.get(), buffer);
    }
  }
}

static Uint32
EDITOR_EVENT()
{
  static Uint32 event = SDL_RegisterEvents(1);
  return event;
}

void
EditorAppImpl::setupInitialBuffers(const EditorInitSettings& settings)
{
  std::shared_ptr<PictureBuffer> buffer;
  if (!settings.filename.empty() ||
      (settings.pictureSz.x > 0 && settings.pictureSz.y > 0)) {
    if (!settings.filename.empty()) {
      buffer = PictureBuffer::load(settings.filename);
    } else {
      buffer = std::make_shared<PictureBuffer>(
        "", Surface(settings.pictureSz, DEFAULT_FORMAT));
    }
    currentView().setBuffer(buffer);
    buffers.emplace_back(buffer);
    bufferIndex = int(buffers.size() - 1);
  }
  currentView().canvas | ColorA{0, 0, 0, 255};
  currentView().canvas | ColorB{255, 255, 255, 255};
  currentView().setToolId(tools::FREE_HAND);
  pushAction(actions::EDITOR_FOCUS_PICTURE);
}

EditorAppImpl::EditorAppImpl(SDL::Window w,
                             SDL::Renderer r,
                             const Rect& pictureViewport)
  : EditorState{.window{std::move(w)},
                .renderer{std::move(r)},
                .pictureViewport{pictureViewport},
                .view{pictureViewport},
                .actions{EDITOR_EVENT()}}
{
}

std::unique_ptr<EditorApp>
createEditorApp(const EditorInitSettings& settings)
{
  if (!SDL_Init(SDL::INIT_VIDEO)) throw std::runtime_error{SDL_GetError()};
  auto [window, renderer] =
    SDL::CreateWindowAndRenderer("Pixedit viewer", settings.windowSz);
  Rect pictureViewport = {0, 0, settings.windowSz.x, settings.windowSz.y};
  auto editorApp = std::make_unique<EditorAppImpl>(
    std::move(window), std::move(renderer), pictureViewport);
  ctx = editorApp.get();

  editorApp->setupActions();
  editorApp->setupInitialBuffers(settings);
  installShortcutPlugin(editorApp->plugins, editorApp->shortcuts);
  installShortcutDefaultsPlugin(editorApp->plugins);

  editorApp->auxWindows.set(
    "core.bufferSelectionWindow",
    initBufferSelectionAuxWindow(&ctx->buffers, ctx->maximizeView));
  editorApp->auxWindows.set("core.pictureOptionsWindow",
                            pictureOptionsAuxWindow);
  return editorApp;
}

SDL::AppResult
EditorAppImpl::Iterate()
{
  update();

  // Render
  renderer.SetDrawColor({60, 60, 60, 255});
  renderer.RenderClear();

  if (maximizeView) picture.render(&view, renderer.get(), pictureViewport);

  ui.render();

  renderer.Present();
  return exited ? SDL::APP_SUCCESS : SDL::APP_CONTINUE;
}

SDL::AppResult
EditorAppImpl::Event(const SDL::Event& e)
{
  ui.event(e);
  switch (e.type) {
  case SDL_EVENT_QUIT:
    if (buffers.empty() || !defaults::ASK_SAVE_ON_CLOSE) {
      exited = true;
      break;
    }
    exiting = true;
    pushAction(actions::PIC_CLOSE);
    break;
  case SDL_EVENT_WINDOW_RESIZED:
    pictureViewport.w = e.window.data1 - pictureViewport.x;
    pictureViewport.h = e.window.data2 - pictureViewport.y;
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    if (ImGui::GetIO().WantCaptureMouse || !maximizeView) break;
    view.state.wheelX += e.wheel.x;
    view.state.wheelY += e.wheel.y;
    break;
  case SDL_EVENT_DROP_FILE:
    if (ImGui::GetIO().WantCaptureMouse) break;
    appendFile(PictureBuffer::load(e.drop.data));
    break;
  case SDL_EVENT_KEY_DOWN: {
    if (ImGui::GetIO().WantCaptureKeyboard) break;
    auto mod = e.key.mod;
    if (auto action = shortcuts.get({.key = e.key.key,
                                     .ctrl = (mod & SDL_KMOD_CTRL) != 0,
                                     .alt = (mod & SDL_KMOD_ALT) != 0,
                                     .shift = (mod & SDL_KMOD_SHIFT) != 0})) {
      pushAction(*action);
    }
    break;
  }
  default: pixedit::ActionManager::check(e.user); break;
  }
  return SDL::APP_CONTINUE;
}

void
EditorAppImpl::close(bool force = false)
{
  if (buffers.empty()) {
    exited = true;
  } else if (!force && currentBuffer()->isDirty() &&
             defaults::ASK_SAVE_ON_CLOSE) {
    pushAction(actions::MODAL_SHOW, "Confirm exit");
  } else {
    viewSettings.erase(currentBuffer());
    buffers.erase(buffers.begin() + bufferIndex);
    if (bufferIndex >= int(buffers.size())) { bufferIndex -= 1; }
    if (bufferIndex < 0) {
      view.setBuffer(nullptr);
    } else {
      view.setBuffer(currentBuffer());
    }
  }
}

PictureManager&
currentPicture()
{ return ctx->picture; }

ViewSettings&
getSettingsFor(const std::shared_ptr<PictureBuffer>& buffer)
{
  auto& settings = ctx->viewSettings[buffer];
  auto& name = buffer->getName();
  if (buffer->getFilename().empty()) {
    if (settings.filename != name) {
      settings.filename = name;
      settings.titleBuffer =
        std::format("{}###{:x}", name, size_t(buffer.get()));
    }
  } else {
    auto& filename = buffer->getFilename();
    if (filename != settings.filename) {
      settings.filename = filename;
      settings.titleBuffer =
        std::format("{}###{:x}", name, size_t(buffer.get()));
    }
  }
  return settings;
}

void
EditorAppImpl::setupActions()
{
  actions.set(actions::PIC_NEW,
              [&] { pushAction(actions::MODAL_SHOW, "New image"); });
  actions.set(actions::PIC_OPEN, [&] {
    auto buffer = loadFromFileDialog("./");
    if (buffer) { appendFile(buffer); };
  });
  actions.set(actions::PIC_CLOSE, [&] { close(); });
  actions.set(actions::PIC_FORCE_CLOSE, [&] { close(true); });
  actions.set(actions::PIC_SAVE, [&] {
    if (buffers.empty() || bufferIndex < 0) return;
    if (currentBuffer()->getFilename().empty()) {
      saveWithFileDialog(*currentBuffer());
    } else {
      currentBuffer()->save();
    }
  });
  actions.set(actions::PIC_SAVE_AS, [&] {
    if (buffers.empty() || bufferIndex < 0) return;
    saveWithFileDialog(*currentBuffer());
  });
  actions.set(actions::VIEW_CHANGE, [&](auto param) {
    std::stringstream ss{std::string{param}};
    if (!(ss >> bufferIndex) || bufferIndex >= buffers.size()) { return; }
    if (auto lastBuffer = currentBuffer()) {
      auto& settings = viewSettings[lastBuffer];
      settings.view.offset = view.offset;
      settings.view.scale = view.scale;
    }
    auto nextBuffer = currentBuffer();
    auto& settings = viewSettings[nextBuffer];
    view.setBuffer(std::move(nextBuffer));
    view.offset = settings.view.offset;
    view.scale = settings.view.scale;
  });

  actions.set(actions::SELECTION_PERSIST,
              [&] { currentView().persistSelection(); });
  actions.set(actions::SELECTION_DELETE,
              [&] { currentView().setSelection(nullptr); });
  actions.set(actions::CLIP_COPY, [&] {
    if (!currentBuffer()) return;
    auto& buffer = *currentBuffer();
    auto selectionSurface = buffer.getSelectionSurface();
    Clipboard::set(selectionSurface ?: buffer.getSurface());
  });
  actions.set(actions::CLIP_CUT, [&] {
    if (!currentBuffer()) return;
    auto& buffer = *currentBuffer();
    if (!buffer.hasSelection()) return;
    Clipboard::set(buffer.getSelectionSurface());
    currentView().setSelection(nullptr);
  });
  actions.set(actions::CLIP_PASTE, [&] {
    auto buffer = currentBuffer();
    if (!buffer) {
      pushAction(actions::CLIP_PASTE_NEW);
      return;
    }
    auto surface = Clipboard::get();
    if (!surface) return;
    currentView().setSelection(surface);
  });
  actions.set(actions::CLIP_PASTE_NEW, [&] {
    auto surface = Clipboard::get();
    if (!surface) return;
    appendFile(std::make_shared<PictureBuffer>("", surface, true));
  });

  actions.set(actions::HISTORY_UNDO, [&] { currentView().undo(); });
  actions.set(actions::HISTORY_REDO, [&] { currentView().redo(); });
  actions.set(actions::EDITOR_COLOR_SWAP,
              [&] { currentPicture().swapColors(); });
  actions.set(actions::VIEW_GRID_TOGGLE, [&] {
    currentView().enableGrid(!currentView().isGridEnabled());
    pushAction(actions::EDITOR_FOCUS_PICTURE);
  });
  actions.set(actions::EDITOR_FOCUS_PICTURE,
              [&] { focusBufferNextFrame = true; });

  actions.set(actions::MODAL_SHOW,
              [&](Id parameter) { requestModal = parameter; });
}

void
appendFile(const std::shared_ptr<PictureBuffer>& buffer)
{
  ctx->view.setBuffer(buffer);
  ctx->view.offset = {0, 0};
  ctx->view.scale = 1.f;
  ctx->buffers.emplace_back(buffer);
  ctx->bufferIndex = int(ctx->buffers.size() - 1);
}

std::shared_ptr<PictureBuffer>
currentBuffer()
{
  if (ctx->bufferIndex < 0 || ctx->buffers.empty()) return nullptr;
  return ctx->buffers[ctx->bufferIndex];
}

PictureView&
currentView()
{
  if (!ctx->maximizeView && currentBuffer()) {
    return ctx->viewSettings[currentBuffer()].view;
  }
  return ctx->view;
}

} // namespace pixedit
