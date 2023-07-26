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
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  PluginManager plugins;
  ImGuiComponent ui;

  PictureManager picture;
  Rect pictureViewport;
  PictureView view;
  bool exited = false;
  bool maximizeView = false;

  std::vector<std::shared_ptr<PictureBuffer>> buffers;
  std::map<std::shared_ptr<PictureBuffer>, ViewSettings> viewSettings;
  int bufferIndex = -1;

  Clipboard clipboard;
  ActionManager actions;
  ShortcutManager shortcuts;
  AuxWindowManager auxWindows;

  bool exiting = false;
  bool focusBufferNextFrame = true;
  std::string requestModal;
};

static EditorState* ctx = nullptr;

static void
setupActions();
static void
event(const SDL_Event& ev, bool imGuiMayUse);

void
update()
{
  if (ctx->maximizeView) {
    if (!ImGui::GetIO().WantCaptureMouse &&
        SDL_GetMouseFocus() == ctx->window) {
      auto buttonState =
        SDL_GetMouseState(&ctx->view.state.x, &ctx->view.state.y);
      ctx->view.state.left = buttonState & SDL_BUTTON_LMASK;
      ctx->view.state.middle = buttonState & SDL_BUTTON_MMASK;
      ctx->view.state.right = buttonState & SDL_BUTTON_RMASK;
    };
    ctx->picture.update(&ctx->view, ctx->renderer, ctx->pictureViewport);
  }
  ctx->ui.update();
  showMainMenuBar(currentView(), &ctx->maximizeView);
  ctx->auxWindows.showAll();

  if (!ctx->requestModal.empty()) {
    ImGui::OpenPopup(ctx->requestModal.c_str());
    ctx->requestModal.clear();
  }
  showNewFileDialog();
  showConfirmExitDialog(&ctx->exiting);
  showAboutDialog();

  if (!ctx->maximizeView) {
    for (auto& buffer : ctx->buffers) {
      if (ctx->focusBufferNextFrame && buffer == currentBuffer()) {
        ctx->focusBufferNextFrame = false;
        ImGui::SetNextWindowFocus();
      }
      showPictureWindow(ctx->renderer, buffer);
    }
  }
}

static SDL_Window*
makeWindow(SDL_Point windowSz)
{
  auto window = SDL_CreateWindow("Pixedit viewer",
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 windowSz.x,
                                 windowSz.y,
                                 SDL_WINDOW_RESIZABLE);
  if (!window) { throw std::runtime_error{SDL_GetError()}; }
  return window;
}

static SDL_Renderer*
makeRenderer(SDL_Window* window)
{
  auto renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) { throw std::runtime_error{SDL_GetError()}; }
  return renderer;
}

static Uint32
EDITOR_EVENT()
{
  static Uint32 event = SDL_RegisterEvents(1);
  return event;
}

static void
setupInitialBuffers(const EditorInitSettings& settings)
{
  std::shared_ptr<PictureBuffer> buffer;
  if (!settings.filename.empty() ||
      (settings.pictureSz.x > 0 && settings.pictureSz.y > 0)) {
    if (!settings.filename.empty()) {
      buffer = PictureBuffer::load(settings.filename);
    } else {
      buffer = std::make_shared<PictureBuffer>(
        "", Surface::create(settings.pictureSz.x, settings.pictureSz.y));
    }
    currentView().setBuffer(buffer);
    ctx->buffers.emplace_back(buffer);
    ctx->bufferIndex = ctx->buffers.size() - 1;
  }
  currentView().canvas | ColorA{0, 0, 0, 255};
  currentView().canvas | ColorB{255, 255, 255, 255};
  currentView().setToolId(tools::FREE_HAND);
  pushAction(actions::EDITOR_FOCUS_PICTURE);
}

int
runEditorApp(EditorInitSettings settings)
{
  if (SDL_Init(SDL_INIT_VIDEO)) throw std::runtime_error{SDL_GetError()};
  auto window = makeWindow(settings.windowSz);
  auto renderer = makeRenderer(window);
  Rect pictureViewport = {0, 0, settings.windowSz.x, settings.windowSz.y};
  EditorState state{
    .window{window},
    .renderer{renderer},
    .ui{window, renderer},
    .pictureViewport{pictureViewport},
    .view{pictureViewport},
    .actions{EDITOR_EVENT()},
  };
  ctx = &state;

  setupActions();
  setupInitialBuffers(settings);
  installShortcutPlugin(ctx->plugins, ctx->shortcuts);
  installShortcutDefaultsPlugin(ctx->plugins);

  ctx->auxWindows.set(
    "core.bufferSelectionWindow",
    initBufferSelectionAuxWindow(&ctx->buffers, ctx->maximizeView));
  ctx->auxWindows.set("core.pictureOptionsWindow", pictureOptionsAuxWindow);

  while (!ctx->exited) {
    // Update
    for (SDL_Event ev; SDL_PollEvent(&ev);) { event(ev, ctx->ui.event(ev)); }

    update();

    /// Render
    SDL_SetRenderDrawColor(ctx->renderer, 60, 60, 60, 255);
    SDL_RenderClear(ctx->renderer);

    if (ctx->maximizeView) {
      ctx->picture.render(&ctx->view, ctx->renderer, ctx->pictureViewport);
    }

    ctx->ui.render();

    SDL_RenderPresent(ctx->renderer);
    SDL_Delay(10);
  }
  ctx = nullptr;
  SDL_Quit();
  return EXIT_SUCCESS;
}

void
event(const SDL_Event& ev, bool imGuiMayUse)
{
  switch (ev.type) {
  case SDL_QUIT:
    if (ctx->buffers.empty() || !defaults::ASK_SAVE_ON_CLOSE) {
      ctx->exited = true;
      break;
    }
    ctx->exiting = true;
    pushAction(actions::PIC_CLOSE);
    return;
  case SDL_WINDOWEVENT:
    switch (ev.window.event) {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      ctx->pictureViewport.w = ev.window.data1 - ctx->pictureViewport.x;
      ctx->pictureViewport.h = ev.window.data2 - ctx->pictureViewport.y;
      break;
    default: break;
    }
    break;
  case SDL_MOUSEWHEEL:
    if (ImGui::GetIO().WantCaptureMouse || !ctx->maximizeView) break;
    ctx->view.state.wheelX += ev.wheel.x;
    ctx->view.state.wheelY += ev.wheel.y;
    break;
  case SDL_DROPFILE:
    if (ImGui::GetIO().WantCaptureMouse) break;
    appendFile(PictureBuffer::load(ev.drop.file));
    break;
  case SDL_KEYDOWN: {
    if (ImGui::GetIO().WantCaptureKeyboard) break;
    auto mod = ev.key.keysym.mod;
    if (auto action = ctx->shortcuts.get({.key = ev.key.keysym.sym,
                                          .ctrl = (mod & KMOD_CTRL) != 0,
                                          .alt = (mod & KMOD_ALT) != 0,
                                          .shift = (mod & KMOD_SHIFT) != 0})) {
      pushAction(*action);
    }
    break;
  }
  default: ctx->actions.check(ev.user); break;
  }
}

static void
close(bool force = false)
{
  if (ctx->buffers.empty()) {
    ctx->exited = true;
  } else if (!force && currentBuffer()->isDirty() &&
             defaults::ASK_SAVE_ON_CLOSE) {
    pushAction(actions::MODAL_SHOW, "Confirm exit");
  } else {
    ctx->viewSettings.erase(currentBuffer());
    ctx->buffers.erase(ctx->buffers.begin() + ctx->bufferIndex);
    if (ctx->bufferIndex >= int(ctx->buffers.size())) { ctx->bufferIndex -= 1; }
    if (ctx->bufferIndex < 0) {
      ctx->view.setBuffer(nullptr);
    } else {
      ctx->view.setBuffer(currentBuffer());
    }
  }
}

PictureManager&
currentPicture()
{
  return ctx->picture;
}

ViewSettings&
getSettingsFor(const std::shared_ptr<PictureBuffer>& buffer)
{
  auto& settings = ctx->viewSettings[buffer];
  if (buffer->getFilename().empty()) {
    if (settings.fileUnamedId == 0) {
      int largestUnnamedId = 0;
      for (auto& s : ctx->viewSettings) {
        if (s.second.fileUnamedId > largestUnnamedId) {
          largestUnnamedId = s.second.fileUnamedId;
        }
      }
      settings.fileUnamedId = largestUnnamedId + 1;
      std::stringstream ss;
      ss << "New image " << settings.fileUnamedId;
      settings.filename = ss.str();
      ss << "###" << buffer.get();
      settings.titleBuffer = ss.str();
    }
  } else {
    auto& filename = buffer->getFilename();
    if (filename != settings.filename) {
      settings.filename = filename;
      settings.fileUnamedId = 0;

      auto lastSlash = filename.find_last_of('/');
      if (lastSlash == std::string::npos) {
        settings.titleBuffer = filename;
      } else {
        std::stringstream ss;
        ss << filename.substr(lastSlash + 1) << "###" << buffer.get();
        settings.titleBuffer = ss.str();
      }
    }
  }
  return settings;
}

void
setupActions()
{
  auto& actions = ctx->actions;
  actions.set(actions::PIC_NEW,
              [&] { pushAction(actions::MODAL_SHOW, "New image"); });
  actions.set(actions::PIC_OPEN, [&] {
    auto buffer = loadFromFileDialog("./");
    if (buffer) { appendFile(buffer); };
  });
  actions.set(actions::PIC_CLOSE, [&] { close(); });
  actions.set(actions::PIC_FORCE_CLOSE, [&] { close(true); });
  actions.set(actions::PIC_SAVE, [&] {
    if (ctx->buffers.empty() || ctx->bufferIndex < 0) return;
    if (currentBuffer()->getFilename().empty()) {
      saveWithFileDialog(*currentBuffer());
    } else {
      currentBuffer()->save();
    }
  });
  actions.set(actions::PIC_SAVE_AS, [&] {
    if (ctx->buffers.empty() || ctx->bufferIndex < 0) return;
    saveWithFileDialog(*currentBuffer());
  });
  actions.set(actions::VIEW_CHANGE, [&](auto param) {
    std::stringstream ss{std::string{param}};
    int bufferIndex = -1;
    ss >> bufferIndex;
    if (bufferIndex < 0 || bufferIndex >= ctx->buffers.size()) { return; }
    if (auto lastBuffer = currentBuffer()) {
      auto& settings = ctx->viewSettings[lastBuffer];
      settings.view.offset = ctx->view.offset;
      settings.view.scale = ctx->view.scale;
    }
    ctx->bufferIndex = bufferIndex;
    auto nextBuffer = currentBuffer();
    auto& settings = ctx->viewSettings[nextBuffer];
    ctx->view.setBuffer(std::move(nextBuffer));
    ctx->view.offset = settings.view.offset;
    ctx->view.scale = settings.view.scale;
  });

  actions.set(actions::SELECTION_PERSIST,
              [&] { currentView().persistSelection(); });
  actions.set(actions::SELECTION_DELETE,
              [&] { currentView().setSelection(nullptr); });
  actions.set(actions::CLIP_COPY, [&] {
    if (!currentBuffer()) return;
    auto& buffer = *currentBuffer();
    auto selectionSurface = buffer.getSelectionSurface();
    ctx->clipboard.set(selectionSurface ?: buffer.getSurface());
  });
  actions.set(actions::CLIP_CUT, [&] {
    if (!currentBuffer()) return;
    auto& buffer = *currentBuffer();
    if (!buffer.hasSelection()) return;
    ctx->clipboard.set(buffer.getSelectionSurface());
    currentView().setSelection(nullptr);
  });
  actions.set(actions::CLIP_PASTE, [&] {
    auto buffer = currentBuffer();
    if (!buffer) {
      pushAction(actions::CLIP_PASTE_NEW);
      return;
    }
    auto surface = ctx->clipboard.get();
    if (!surface) return;
    currentView().setSelection(surface);
  });
  actions.set(actions::CLIP_PASTE_NEW, [&] {
    auto surface = ctx->clipboard.get();
    if (!surface) return; // TODO Error?
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
              [&] { ctx->focusBufferNextFrame = true; });

  actions.set(actions::MODAL_SHOW,
              [&](Id parameter) { ctx->requestModal = parameter; });
}

void
appendFile(std::shared_ptr<PictureBuffer> buffer)
{
  ctx->view.setBuffer(buffer);
  ctx->view.offset = {0, 0};
  ctx->view.scale = 1.f;
  ctx->buffers.emplace_back(buffer);
  ctx->bufferIndex = ctx->buffers.size() - 1;
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
