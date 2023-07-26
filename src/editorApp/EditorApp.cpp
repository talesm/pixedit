#include "EditorApp.hpp"
#include <sstream>
#include <stdexcept>
#include <imgui.h>
#include "Action.hpp"
#include "Clipboard.hpp"
#include "FileDialogTinyfd.hpp"
#include "ImGuiComponent.hpp"
#include "PictureManager.hpp"
#include "PictureView.hpp"
#include "PluginManager.hpp"
#include "actions.hpp"
#include "imgui/showAboutDialog.hpp"
#include "imgui/showMainMenu.hpp"
#include "imgui/showNewFileDialog.hpp"
#include "shortcutPlugin.hpp"
#include "tools.hpp"

namespace pixedit {

namespace defaults {
extern const bool ASK_SAVE_ON_CLOSE;
} // namespace defaults

struct ViewSettings
{
  PictureView view;
  SDL_Texture* texture = nullptr;
  int fileUnamedId = 0;
  std::string filename;
  std::string titleBuffer;
};

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

  bool exiting = false;
  bool focusBufferNextFrame = true;
  std::string requestModal;
};

static EditorState* ctx = nullptr;

static void
setupActions();
static void
event(const SDL_Event& ev, bool imGuiMayUse);
static void
update();
static void
showConfirmExitDialog();
static void
showPictureWindows();
static void
showPictureWindow(const std::shared_ptr<PictureBuffer>& buffer);
static void
showPictureOptions();

/// Actions
static void
close(bool force = false);
static std::shared_ptr<PictureBuffer>
currentBuffer();
static PictureView&
currentView();

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
  if (ImGui::Begin("Picture options")) { showPictureOptions(); }
  ImGui::End();

  if (!ctx->requestModal.empty()) {
    ImGui::OpenPopup(ctx->requestModal.c_str());
    ctx->requestModal.clear();
  }
  showNewFileDialog();
  showConfirmExitDialog();
  showAboutDialog();

  if (!ctx->maximizeView) { showPictureWindows(); }
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

EditorApp::EditorApp(EditorInitSettings settings)
{
  SDL_assert_always(ctx == nullptr);
  if (SDL_Init(SDL_INIT_VIDEO)) throw std::runtime_error{SDL_GetError()};
  auto window = makeWindow(settings.windowSz);
  auto renderer = makeRenderer(window);
  Rect pictureViewport = {0, 0, settings.windowSz.x, settings.windowSz.y};
  ctx = new EditorState{
    .window{window},
    .renderer{renderer},
    .ui{window, renderer},
    .pictureViewport{pictureViewport},
    .view{pictureViewport},
    .actions{EDITOR_EVENT()},
  };
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

  setupActions();

  installShortcutPlugin(ctx->plugins, ctx->shortcuts);
  installShortcutDefaultsPlugin(ctx->plugins);

  currentView().canvas | ColorA{0, 0, 0, 255};
  currentView().canvas | ColorB{255, 255, 255, 255};
  currentView().setToolId(tools::FREE_HAND);
  pushAction(actions::EDITOR_FOCUS_PICTURE);
}

EditorApp::~EditorApp()
{
  delete ctx;
  ctx = nullptr;
  SDL_Quit();
}

int
EditorApp::run()
{
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

void
close(bool force)
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

inline bool
PatternCombo(const char* label, Pattern* pattern)
{
  static std::map<Uint64, const char*> pNames{
    {patterns::SOLID.data8x8, "Solid"},
    {patterns::CHECKERED.data8x8, "Checkered"},
    {patterns::CHECKERED_2.data8x8, "Checkered 2x2"},
    {patterns::CHECKERED_4.data8x8, "Checkered 4x4"},
  };
  bool result = false;
  if (ImGui::BeginCombo(label, pNames[pattern->data8x8] ?: "????")) {
    for (auto& e : pNames) {
      bool isSelected = e.first == pattern->data8x8;
      if (ImGui::Selectable(e.second, isSelected)) {
        pattern->data8x8 = e.first;
        result = true;
      }
      if (isSelected) { ImGui::SetItemDefaultFocus(); }
    }
    ImGui::EndCombo();
  }
  return result;
}

void
showPictureOptions()
{
  ImGui::BeginDisabled(ctx->maximizeView == false);
  if (ImGui::BeginCombo("File",
                        ctx->bufferIndex < 0
                          ? "None"
                          : currentBuffer()->getFilename().c_str())) {
    int i = 0;
    for (auto& b : ctx->buffers) {
      bool selected = ctx->bufferIndex == i;
      if (ImGui::Selectable(b->getFilename().c_str(), selected)) {
        ctx->bufferIndex = i;
        if (ctx->bufferIndex >= 0) {
          auto& settings = ctx->viewSettings[currentBuffer()];
          settings.view.offset = ctx->view.offset;
          settings.view.scale = ctx->view.scale;
        }
        auto nextBuffer = currentBuffer();
        auto& settings = ctx->viewSettings[nextBuffer];
        ctx->view.setBuffer(std::move(nextBuffer));
        ctx->view.offset = settings.view.offset;
        ctx->view.scale = settings.view.scale;
      }
      if (selected) { ImGui::SetItemDefaultFocus(); }
      ++i;
    }
    ImGui::EndCombo();
  }
  ImGui::EndDisabled();

  if (ImGui::CollapsingHeader("Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
    auto currId = currentView().getToolId();
    for (auto& tool : getTools()) {
      if (ImGui::RadioButton(tool.name.c_str(), tool.id == currId)) {
        currentView().setToolId(tool.id);
        pushAction(actions::EDITOR_FOCUS_PICTURE);
      }
    }
  }
  ImGui::DragFloat2("##offset", &currentView().offset.x, 1.f, -10000, +10000);
  ImGui::SameLine();
  if (ImGui::Button("Reset##offset")) { currentView().offset = {0}; }
  if (ImGui::ArrowButton("Decrease zoom", ImGuiDir_Left)) {
    currentView().scale /= 2;
    pushAction(actions::EDITOR_FOCUS_PICTURE);
  }
  ImGui::SameLine();
  ImGui::Text("%g%%", currentView().scale * 100);
  ImGui::SameLine();
  if (ImGui::ArrowButton("Increase zoom", ImGuiDir_Right)) {
    currentView().scale *= 2;
    pushAction(actions::EDITOR_FOCUS_PICTURE);
  }

  float colorAreaHeight =
    ImGui::GetFrameHeightWithSpacing() + ImGui::GetFrameHeight();
  if (ImGui::Button("Swap colors", {0, colorAreaHeight})) {
    pushAction(actions::EDITOR_COLOR_SWAP);
    pushAction(actions::EDITOR_FOCUS_PICTURE);
  }
  ImGui::SameLine();
  {
    ImGui::BeginChild("Colors", {0, colorAreaHeight});
    auto colorA = componentToNormalized(ctx->picture.getColorA());
    if (ImGui::ColorEdit4(
          "Color A", colorA.data(), ImGuiColorEditFlags_NoInputs)) {
      ctx->picture.setColorA(normalizedToComponent(colorA));
    }
    auto colorB = componentToNormalized(ctx->picture.getColorB());
    if (ImGui::ColorEdit4(
          "Color B", colorB.data(), ImGuiColorEditFlags_NoInputs)) {
      ctx->picture.setColorA(normalizedToComponent(colorB));
    }
    ImGui::EndChild();
  }
  auto brush = ctx->picture.getBrush();
  int penSize = brush.pen.w;
  if (ImGui::SliderInt("Pen size", &penSize, 1, 16)) {
    brush.pen = Pen{penSize, penSize};
    ctx->picture.setBrush(brush);
  }
  Pattern pattern = brush.pattern;
  if (PatternCombo("Tile: ", &pattern)) {
    brush.pattern = pattern;
    ctx->picture.setBrush(brush);
    pushAction(actions::EDITOR_FOCUS_PICTURE);
  }
  if (ImGui::CollapsingHeader("Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
    bool transparent = ctx->picture.isTransparent();
    if (ImGui::Checkbox("Transparent", &transparent)) {
      ctx->picture.setTransparent(transparent);
      pushAction(actions::EDITOR_FOCUS_PICTURE);
    }
    ImGui::Checkbox("Fill selected out region", &currentView().fillSelectedOut);
  }
}

void
showPictureWindows()
{
  for (auto& buffer : ctx->buffers) { showPictureWindow(buffer); }
}

void
showPictureWindow(const std::shared_ptr<PictureBuffer>& buffer)
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
  ImGuiWindowFlags flags = 0;
  if (buffer->isDirty()) { flags |= ImGuiWindowFlags_UnsavedDocument; }
  // TODO Measure title and decoration instead of guessing
  ImGui::SetNextWindowSize(ImVec2(buffer->getW() + 16, buffer->getH() + 35),
                           ImGuiCond_Once);
  if (ctx->focusBufferNextFrame && buffer == currentBuffer()) {
    ctx->focusBufferNextFrame = false;
    ImGui::SetNextWindowFocus();
  }
  bool stayOpen = true;
  if (ImGui::Begin(settings.titleBuffer.c_str(), &stayOpen, flags)) {
    bool redraw = false;
    ImVec2 canvasSz = ImGui::GetContentRegionAvail();
    if (canvasSz.x < 50.0f) canvasSz.x = 50.0f;
    if (canvasSz.y < 50.0f) canvasSz.y = 50.0f;
    {
      int picW, picH;
      SDL_QueryTexture(settings.texture, nullptr, nullptr, &picW, &picH);
      if (settings.texture == nullptr || picW != canvasSz.x ||
          picH != canvasSz.y) {
        SDL_DestroyTexture(settings.texture);
        settings.texture = SDL_CreateTexture(ctx->renderer,
                                             SDL_PIXELFORMAT_ABGR32,
                                             SDL_TEXTUREACCESS_TARGET,
                                             canvasSz.x,
                                             canvasSz.y);
        SDL_SetTextureBlendMode(settings.texture, SDL_BLENDMODE_BLEND);
        redraw = true;
      }
    }
    ImVec2 canvasP0 = ImGui::GetCursorScreenPos();
    ImVec2 canvasP1 = ImVec2(canvasP0.x + canvasSz.x, canvasP0.y + canvasSz.y);

    ImGuiIO& io = ImGui::GetIO();

    auto& view = settings.view;
    if (ImGui::IsWindowFocused()) {
      ImGui::InvisibleButton("Canvas",
                             canvasSz,
                             ImGuiButtonFlags_MouseButtonLeft |
                               ImGuiButtonFlags_MouseButtonRight |
                               ImGuiButtonFlags_MouseButtonMiddle);
      const bool is_hovered = ImGui::IsItemHovered(); // Hovered
      const bool is_active = ImGui::IsItemActive();   // Held
      if (is_hovered || is_active) {
        view.state.x = io.MousePos.x - canvasP0.x;
        view.state.y = io.MousePos.y - canvasP0.y;
        view.state.left = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        view.state.middle = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
        view.state.right = ImGui::IsMouseDown(ImGuiMouseButton_Right);
        view.state.wheelX += io.MouseWheelH;
        view.state.wheelY += io.MouseWheel;
      }
    }
    if (ImGui::IsWindowFocused() || redraw) {
      view.setBuffer(buffer);
      SDL_SetRenderTarget(ctx->renderer, settings.texture);
      SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 0);
      SDL_RenderClear(ctx->renderer);
      ctx->picture.update(
        &view, ctx->renderer, {0, 0, int(canvasSz.x), int(canvasSz.y)});
      ctx->picture.render(
        &view, ctx->renderer, {0, 0, int(canvasSz.x), int(canvasSz.y)});

      SDL_SetRenderTarget(ctx->renderer, nullptr);
    }
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddImage(settings.texture, canvasP0, canvasP1);
  }
  if (!stayOpen) { pushAction(actions::PIC_CLOSE); }
  ImGui::End();
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
  actions.set(actions::EDITOR_COLOR_SWAP, [&] { currentView().swapColors(); });
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
showConfirmExitDialog()
{
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (ImGui::BeginPopupModal(
        "Confirm exit", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Image unsaved changes will be lost?");
    const char* cstr = "";
    if (ctx->bufferIndex >= 0) {
      auto buffer = currentBuffer();
      if (buffer && !buffer->getFilename().empty()) {
        cstr = buffer->getFilename().c_str();
      } else {
        auto& settings = ctx->viewSettings[buffer];
        cstr = settings.filename.c_str();
      }
    }
    ImGui::Text("File: %s", cstr);
    ImGui::Separator();

    if (ImGui::Button("Close",
                      ImVec2(ImGui::GetContentRegionAvail().x * .45f, 0))) {
      close(true);
      if (ctx->exiting) {
        pushAction(actions::PIC_CLOSE);
      } else {
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
      ImGui::CloseCurrentPopup();
      ctx->exiting = false;
    }
    ImGui::SetItemDefaultFocus();
    ImGui::EndPopup();
  } else if (ctx->exiting) {
    pushAction(actions::PIC_CLOSE);
  }
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
