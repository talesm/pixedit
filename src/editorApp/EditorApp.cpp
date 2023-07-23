#include "EditorApp.hpp"
#include <sstream>
#include <stdexcept>
#include <imgui.h>
#include "FileDialogTinyfd.hpp"
#include "actions.hpp"
#include "tools.hpp"

namespace pixedit {

namespace defaults {
extern const bool ASK_SAVE_ON_CLOSE;
} // namespace defaults

void
EditorApp::update()
{
  if (maximizeView) {
    if (!ImGui::GetIO().WantCaptureMouse && SDL_GetMouseFocus() == window) {
      auto buttonState = SDL_GetMouseState(&view.state.x, &view.state.y);
      view.state.left = buttonState & SDL_BUTTON_LMASK;
      view.state.middle = buttonState & SDL_BUTTON_MMASK;
      view.state.right = buttonState & SDL_BUTTON_RMASK;
    };
    picture.update(&view, renderer, pictureViewport);
  }
  ui.update();
  showMainMenuBar();
  if (ImGui::Begin("Picture options")) { showPictureOptions(); }
  ImGui::End();
  if (!maximizeView) { showPictureWindows(); }
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
  : window(makeWindow(settings.windowSz))
  , renderer(makeRenderer(window))
  , ui(window, renderer)
  , pictureViewport{0, 0, settings.windowSz.x, settings.windowSz.y}
  , view{pictureViewport}
  , actions{EDITOR_EVENT()}
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
    buffers.emplace_back(buffer);
    bufferIndex = buffers.size() - 1;
  }

  setupActions();
  setupShortcuts();

  currentView().canvas | ColorA{0, 0, 0, 255};
  currentView().canvas | ColorB{255, 255, 255, 255};
  currentView().setToolId(tools::FREE_HAND);
  pushAction(actions::EDITOR_FOCUS_PICTURE);
}

int
EditorApp::run()
{
  while (!exited) {
    // Update
    for (SDL_Event ev; SDL_PollEvent(&ev);) { event(ev, ui.event(ev)); }

    update();

    /// Render
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderClear(renderer);

    if (maximizeView) { picture.render(&view, renderer, pictureViewport); }

    ui.render();

    SDL_RenderPresent(renderer);
    SDL_Delay(10);
  }
  SDL_Quit();
  return EXIT_SUCCESS;
}

void
EditorApp::event(const SDL_Event& ev, bool imGuiMayUse)
{
  switch (ev.type) {
  case SDL_QUIT:
    if (buffers.empty() || !defaults::ASK_SAVE_ON_CLOSE) {
      exited = true;
      break;
    }
    exiting = true;
    pushAction(actions::PIC_CLOSE);
    return;
  case SDL_WINDOWEVENT:
    switch (ev.window.event) {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      pictureViewport.w = ev.window.data1 - pictureViewport.x;
      pictureViewport.h = ev.window.data2 - pictureViewport.y;
      break;
    default: break;
    }
    break;
  case SDL_MOUSEWHEEL:
    if (ImGui::GetIO().WantCaptureMouse || !maximizeView) break;
    view.state.wheelX += ev.wheel.x;
    view.state.wheelY += ev.wheel.y;
    break;
  case SDL_DROPFILE:
    if (ImGui::GetIO().WantCaptureMouse) break;
    appendFile(PictureBuffer::load(ev.drop.file));
    break;
  case SDL_KEYDOWN: {
    if (ImGui::GetIO().WantCaptureKeyboard) break;
    auto mod = ev.key.keysym.mod;
    if (auto action = shortcuts.get({.key = ev.key.keysym.sym,
                                     .ctrl = (mod & KMOD_CTRL) != 0,
                                     .alt = (mod & KMOD_ALT) != 0,
                                     .shift = (mod & KMOD_SHIFT) != 0})) {
      pushAction(*action);
    }
    break;
  }
  default: actions.check(ev.user); break;
  }
}

void
EditorApp::close(bool force)
{
  if (buffers.empty()) {
    exited = true;
  } else if (!force && currentBuffer()->isDirty() &&
             defaults::ASK_SAVE_ON_CLOSE) {
    requestModal = "Confirm exit";
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

static void
showAboutDialog()
{
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (ImGui::BeginPopupModal(
        "About", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Pixedit version: v0.1.0-AuspiciousAmber (Alpha)");
    ImGui::Separator();
    ImGui::TextWrapped(
      "A simple and customizable pixel art focused image editor");
    ImGui::Text("Homepage: ");
    ImGui::SameLine();
    static const char url[] = "https://github.com/jungleowl2/pixedit";
    if (ImGui::SmallButton(url)) { ImGui::SetClipboardText(url); }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) &&
        ImGui::BeginTooltip()) {
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted("Click to copy url to clipboard");
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }

    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("Ok", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::EndPopup();
  }
}

void
EditorApp::showPictureOptions()
{
  if (!requestModal.empty()) {
    ImGui::OpenPopup(requestModal.c_str());
    requestModal.clear();
  }
  showNewFileDialog();
  showConfirmExitDialog();
  showAboutDialog();

  ImGui::BeginDisabled(maximizeView == false);
  if (ImGui::BeginCombo(
        "File",
        bufferIndex < 0 ? "None" : currentBuffer()->getFilename().c_str())) {
    int i = 0;
    for (auto& b : buffers) {
      bool selected = bufferIndex == i;
      if (ImGui::Selectable(b->getFilename().c_str(), selected)) {
        bufferIndex = i;
        if (bufferIndex >= 0) {
          auto& settings = viewSettings[currentBuffer()];
          settings.view.offset = view.offset;
          settings.view.scale = view.scale;
        }
        auto nextBuffer = currentBuffer();
        auto& settings = viewSettings[nextBuffer];
        view.setBuffer(std::move(nextBuffer));
        view.offset = settings.view.offset;
        view.scale = settings.view.scale;
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
    auto colorA = componentToNormalized(picture.getColorA());
    if (ImGui::ColorEdit4(
          "Color A", colorA.data(), ImGuiColorEditFlags_NoInputs)) {
      picture.setColorA(normalizedToComponent(colorA));
    }
    auto colorB = componentToNormalized(picture.getColorB());
    if (ImGui::ColorEdit4(
          "Color B", colorB.data(), ImGuiColorEditFlags_NoInputs)) {
      picture.setColorA(normalizedToComponent(colorB));
    }
    ImGui::EndChild();
  }
  auto brush = picture.getBrush();
  int penSize = brush.pen.w;
  if (ImGui::SliderInt("Pen size", &penSize, 1, 16)) {
    brush.pen = Pen{penSize, penSize};
    picture.setBrush(brush);
  }
  Pattern pattern = brush.pattern;
  if (PatternCombo("Tile: ", &pattern)) {
    brush.pattern = pattern;
    picture.setBrush(brush);
    pushAction(actions::EDITOR_FOCUS_PICTURE);
  }
  if (ImGui::CollapsingHeader("Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
    bool transparent = picture.isTransparent();
    if (ImGui::Checkbox("Transparent", &transparent)) {
      picture.setTransparent(transparent);
      pushAction(actions::EDITOR_FOCUS_PICTURE);
    }
    ImGui::Checkbox("Fill selected out region", &currentView().fillSelectedOut);
  }
}

void
EditorApp::showPictureWindow(const std::shared_ptr<PictureBuffer>& buffer)
{
  auto& settings = viewSettings[buffer];
  if (buffer->getFilename().empty()) {
    if (settings.fileUnamedId == 0) {
      int largestUnnamedId = 0;
      for (auto& s : viewSettings) {
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
  if (focusBufferNextFrame && buffer == currentBuffer()) {
    focusBufferNextFrame = false;
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
        settings.texture = SDL_CreateTexture(renderer,
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
      SDL_SetRenderTarget(renderer, settings.texture);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      picture.update(&view, renderer, {0, 0, int(canvasSz.x), int(canvasSz.y)});
      picture.render(&view, renderer, {0, 0, int(canvasSz.x), int(canvasSz.y)});

      SDL_SetRenderTarget(renderer, nullptr);
    }
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddImage(settings.texture, canvasP0, canvasP1);
  }
  if (!stayOpen) { pushAction(actions::PIC_CLOSE); }
  ImGui::End();
}

void
EditorApp::setupActions()
{
  actions.set(actions::PIC_NEW, [&] { requestModal = "New image"; });
  actions.set(actions::PIC_OPEN, [&] {
    auto buffer = loadFromFileDialog("./");
    if (buffer) { appendFile(buffer); };
  });
  actions.set(actions::PIC_CLOSE, [&] { close(); });
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
  actions.set(actions::SELECTION_PERSIST,
              [&] { currentView().persistSelection(); });
  actions.set(actions::SELECTION_DELETE,
              [&] { currentView().setSelection(nullptr); });
  actions.set(actions::CLIP_COPY, [&] {
    if (!currentBuffer()) return;
    auto& buffer = *currentBuffer();
    auto selectionSurface = buffer.getSelectionSurface();
    clipboard.set(selectionSurface ?: buffer.getSurface());
  });
  actions.set(actions::CLIP_CUT, [&] {
    if (!currentBuffer()) return;
    auto& buffer = *currentBuffer();
    if (!buffer.hasSelection()) return;
    clipboard.set(buffer.getSelectionSurface());
    currentView().setSelection(nullptr);
  });
  actions.set(actions::CLIP_PASTE, [&] {
    auto buffer = currentBuffer();
    if (!buffer) {
      pushAction(actions::CLIP_PASTE_NEW);
      return;
    }
    auto surface = clipboard.get();
    if (!surface) return;
    currentView().setSelection(surface);
  });
  actions.set(actions::CLIP_PASTE_NEW, [&] {
    auto surface = clipboard.get();
    if (!surface) return; // TODO Error?
    appendFile(std::make_shared<PictureBuffer>("", surface, true));
  });

  actions.set(actions::HISTORY_UNDO, [&] { currentView().undo(); });
  actions.set(actions::HISTORY_REDO, [&] { currentView().redo(); });
  actions.set(actions::EDITOR_COLOR_SWAP, [&] { currentView().swapColors(); });
  actions.set(actions::VIEW_GRID_TOGGLE, [&] {
    currentView().enableGrid(!currentView().isGridEnabled());
  });
  actions.set(actions::EDITOR_FOCUS_PICTURE,
              [&] { focusBufferNextFrame = true; });
}

void
EditorApp::setupShortcuts()
{
  shortcuts.set({.key = SDLK_n, .ctrl = true}, actions::PIC_NEW);
  shortcuts.set({.key = SDLK_o, .ctrl = true}, actions::PIC_OPEN);
  shortcuts.set({.key = SDLK_ESCAPE}, actions::SELECTION_PERSIST);
  shortcuts.set({.key = SDLK_DELETE}, actions::SELECTION_DELETE);
  shortcuts.set({.key = SDLK_c, .ctrl = true}, actions::CLIP_COPY);
  shortcuts.set({.key = SDLK_x, .ctrl = true}, actions::CLIP_CUT);
  shortcuts.set({.key = SDLK_v, .ctrl = true}, actions::CLIP_PASTE);
  shortcuts.set({.key = SDLK_v, .ctrl = true, .shift = true},
                actions::CLIP_PASTE_NEW);
  shortcuts.set({.key = SDLK_w, .ctrl = true}, actions::PIC_CLOSE);
  shortcuts.set({.key = SDLK_F4, .ctrl = true}, actions::PIC_CLOSE);
  shortcuts.set({.key = SDLK_s, .ctrl = true}, actions::PIC_SAVE);
  shortcuts.set({.key = SDLK_s, .ctrl = true, .shift = true},
                actions::PIC_SAVE_AS);
  shortcuts.set({.key = SDLK_z, .ctrl = true}, actions::HISTORY_UNDO);
  shortcuts.set({.key = SDLK_z, .ctrl = true, .shift = true},
                actions::HISTORY_REDO);

  // Swap colors
  shortcuts.set({.key = SDLK_x, .alt = true}, actions::EDITOR_COLOR_SWAP);

  // grid
  shortcuts.set({.key = SDLK_g, .alt = true}, actions::VIEW_GRID_TOGGLE);
}

static void
showFileMenuContent(bool hasBuffer)
{
  if (ImGui::MenuItem("New")) { pushAction(actions::PIC_NEW); }
  if (ImGui::MenuItem("Open", "Ctrl+O")) { pushAction(actions::PIC_OPEN); }
  if (ImGui::MenuItem("Save", "Ctrl+S")) { pushAction(actions::PIC_SAVE); }
  if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
    pushAction(actions::PIC_SAVE_AS);
  }
  if (ImGui::MenuItem("Close File", "Ctrl+F4", nullptr, hasBuffer)) {
    pushAction(actions::PIC_CLOSE);
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Exit")) { pushAction(actions::QUIT); }
}

static void
showEditMenuContent()
{
  if (ImGui::MenuItem("Undo", "Ctrl+Z")) { pushAction(actions::HISTORY_UNDO); }
  if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z")) {
    pushAction(actions::HISTORY_REDO);
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Cut", "Ctrl+X")) { pushAction(actions::CLIP_CUT); }
  if (ImGui::MenuItem("Copy", "Ctrl+C")) { pushAction(actions::CLIP_COPY); }
  if (ImGui::MenuItem("Paste", "Ctrl+V")) { pushAction(actions::CLIP_PASTE); }
  if (ImGui::MenuItem("Paste as new", "Ctrl+Shift+V")) {
    pushAction(actions::CLIP_PASTE_NEW);
  }
}

void
EditorApp::showMainMenuBar()
{
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      showFileMenuContent(!buffers.empty());
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      showEditMenuContent();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
      ImGui::Checkbox("Maximize", &maximizeView);
      bool gridEnabled = currentView().isGridEnabled();
      if (ImGui::Checkbox("Show grid", &gridEnabled)) {
        currentView().enableGrid(gridEnabled);
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("About")) { requestModal = "About"; }
      ImGui::EndMenu();
    }
    ImGui::Dummy({50, 0});
    ImGui::Text("%s %dx%d",
                getTool(currentView().getToolId()).name.c_str(),
                currentView().state.x,
                currentView().state.y);
    if (currentBuffer()) {
      ImGui::Dummy({20, 0});
      auto& b = *currentBuffer();
      ImGui::Text("%dx%d", b.getW(), b.getH());
      ImGui::Text("%.2f%%", currentView().scale * 100);
    }
    ImGui::EndMainMenuBar();
  }
}

void
EditorApp::showNewFileDialog()
{
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (ImGui::BeginPopupModal(
        "New image", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Do you want to create a new image");
    ImGui::Separator();

    static int width = 320, height = 240;
    if (ImGui::InputInt("width", &width)) {
      if (width < 1) width = 1;
    }
    if (ImGui::InputInt("height", &height)) {
      if (height < 1) height = 1;
    }

    if (ImGui::Button("OK", ImVec2(120, 0))) {
      appendFile(
        std::make_shared<PictureBuffer>("", Surface::create(width, height)));
      ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
    ImGui::EndPopup();
  }
}
void
EditorApp::showConfirmExitDialog()
{
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (ImGui::BeginPopupModal(
        "Confirm exit", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Image unsaved changes will be lost?");
    const char* cstr = "";
    if (bufferIndex >= 0) {
      auto buffer = currentBuffer();
      if (buffer && !buffer->getFilename().empty()) {
        cstr = buffer->getFilename().c_str();
      } else {
        auto& settings = viewSettings[buffer];
        cstr = settings.filename.c_str();
      }
    }
    ImGui::Text("File: %s", cstr);
    ImGui::Separator();

    if (ImGui::Button("Close",
                      ImVec2(ImGui::GetContentRegionAvail().x * .45f, 0))) {
      close(true);
      if (exiting) {
        pushAction(actions::PIC_CLOSE);
      } else {
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
      ImGui::CloseCurrentPopup();
      exiting = false;
    }
    ImGui::SetItemDefaultFocus();
    ImGui::EndPopup();
  } else if (exiting) {
    pushAction(actions::PIC_CLOSE);
  }
}

} // namespace pixedit
