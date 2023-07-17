#include "EditorApp.hpp"
#include <sstream>
#include <stdexcept>
#include <imgui.h>
#include "FileDialogTinyfd.hpp"
#include "tools.hpp"

namespace pixedit {

namespace defaults {
extern const bool ASK_SAVE_ON_CLOSE;
} // namespace defaults

void
EditorApp::update()
{
  showMainMenuBar();
  if (ImGui::Begin("Picture options")) { showPictureOptions(); }
  ImGui::End();
  if (!showView) { showPictureWindows(); }
}

void
EditorApp::focusBufferWindow()
{
  focusBufferNextFrame = true;
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

EditorApp::EditorApp(EditorInitSettings settings)
  : window(makeWindow(settings.windowSz))
  , renderer(makeRenderer(window))
  , ui(window, renderer)
  , view{{0, 30, settings.windowSz.x, settings.windowSz.y}}
{
  std::shared_ptr<PictureBuffer> buffer;
  if (!settings.filename.empty() ||
      (settings.pictureSz.x > 0 && settings.pictureSz.y > 0)) {
    if (!settings.filename.empty()) {
      buffer = std::make_shared<PictureBuffer>(settings.filename);
    } else {
      buffer = std::make_shared<PictureBuffer>(
        "", Surface::create(settings.pictureSz.x, settings.pictureSz.y));
    }
    view.setBuffer(buffer);
    buffers.emplace_back(buffer);
    bufferIndex = buffers.size() - 1;
  }

  setupShortcuts();

  view.canvas | ColorA{0, 0, 0, 255};
  view.canvas | ColorB{255, 255, 255, 255};
  view.setToolId(tools::FREE_HAND);
  focusBufferWindow();
}

int
EditorApp::run()
{
  while (!exited) {
    // Update
    for (SDL_Event ev; SDL_PollEvent(&ev);) { event(ev, ui.event(ev)); }

    ui.update();
    update();

    if (showView) {
      if (!ImGui::GetIO().WantCaptureMouse && SDL_GetMouseFocus() == window) {
        auto buttonState = SDL_GetMouseState(&view.state.x, &view.state.y);
        view.state.left = buttonState & SDL_BUTTON_LMASK;
        view.state.middle = buttonState & SDL_BUTTON_MMASK;
        view.state.right = buttonState & SDL_BUTTON_RMASK;
      };
      view.update(renderer);
    }

    /// Render
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderClear(renderer);

    if (showView) { view.render(renderer); }

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
    close(false);
    return;
  case SDL_WINDOWEVENT:
    switch (ev.window.event) {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      view.setViewport({0, 0, ev.window.data1, ev.window.data2});
      break;
    default: break;
    }
    break;
  case SDL_MOUSEWHEEL:
    if (ImGui::GetIO().WantCaptureMouse || !showView) break;
    view.state.wheelX += ev.wheel.x;
    view.state.wheelY += ev.wheel.y;
    break;
  case SDL_DROPFILE:
    if (ImGui::GetIO().WantCaptureMouse) break;
    appendFile(std::make_shared<PictureBuffer>(ev.drop.file));
    break;
  case SDL_KEYDOWN: {
    if (ImGui::GetIO().WantCaptureKeyboard) break;
    auto mod = ev.key.keysym.mod;
    shortcuts.exec({.key = ev.key.keysym.sym,
                    .ctrl = (mod & KMOD_CTRL) != 0,
                    .alt = (mod & KMOD_ALT) != 0,
                    .shift = (mod & KMOD_SHIFT) != 0});
    break;
  }
  default: break;
  }
}

inline void
EditorApp::copy()
{
  if (!view.getBuffer()) return;
  auto& buffer = *view.getBuffer();
  auto selectionSurface = buffer.getSelectionSurface();
  clipboard.set(selectionSurface ?: buffer.getSurface());
}

inline void
EditorApp::cut()
{
  if (!view.getBuffer()) return;
  auto& buffer = *view.getBuffer();
  if (!buffer.hasSelection()) return;
  clipboard.set(buffer.getSelectionSurface());
  view.setSelection(nullptr);
}

void
EditorApp::paste()
{
  auto& buffer = view.getBuffer();
  if (!buffer) {
    pasteAsNew();
    return;
  }
  auto surface = clipboard.get();
  if (!surface) return;
  view.setSelection(surface);
}
void
EditorApp::pasteAsNew()
{
  auto surface = clipboard.get();
  if (!surface) return; // TODO Error?
  appendFile(std::make_shared<PictureBuffer>("", surface));
}

void
EditorApp::close(bool force)
{
  if (buffers.empty()) {
    exited = true;
  } else if (!force && buffers[bufferIndex]->isDirty() &&
             defaults::ASK_SAVE_ON_CLOSE) {
    requestModal = "Confirm exit";
  } else {
    buffers.erase(buffers.begin() + bufferIndex);
    if (bufferIndex >= int(buffers.size())) { bufferIndex -= 1; }
    if (bufferIndex < 0) {
      view.setBuffer(nullptr);
    } else {
      viewSettings.erase(view.getBuffer().get());
      if (showView) { view.setBuffer(buffers[bufferIndex]); }
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
    ImGui::Text("Pixedit version: DEVELOPMENT");
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

  ImGui::BeginDisabled(showView == false);
  if (ImGui::BeginCombo("File",
                        bufferIndex < 0
                          ? "None"
                          : buffers[bufferIndex]->getFilename().c_str())) {
    int i = 0;
    for (auto& b : buffers) {
      bool selected = bufferIndex == i;
      if (ImGui::Selectable(b->getFilename().c_str(), selected)) {
        bufferIndex = i;
        if (bufferIndex >= 0) {
          auto& settings = viewSettings[view.getBuffer().get()];
          settings.offset = view.offset;
          settings.scale = view.scale;
        }
        auto nextBuffer = buffers[bufferIndex];
        auto& settings = viewSettings[nextBuffer.get()];
        view.setBuffer(std::move(nextBuffer));
        view.offset = settings.offset;
        view.scale = settings.scale;
      }
      if (selected) { ImGui::SetItemDefaultFocus(); }
      ++i;
    }
    ImGui::EndCombo();
  }
  ImGui::EndDisabled();

  if (ImGui::CollapsingHeader("Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
    auto currId = view.getToolId();
    for (auto& tool : getTools()) {
      if (ImGui::RadioButton(tool.name.c_str(), tool.id == currId)) {
        view.setToolId(tool.id);
        focusBufferWindow();
      }
    }
  }
  ImGui::DragFloat2("##offset", &view.offset.x, 1.f, -10000, +10000);
  ImGui::SameLine();
  if (ImGui::Button("Reset##offset")) { view.offset = {0}; }
  if (ImGui::ArrowButton("Decrease zoom", ImGuiDir_Left)) {
    view.scale /= 2;
    focusBufferWindow();
  }
  ImGui::SameLine();
  ImGui::Text("%g%%", view.scale * 100);
  ImGui::SameLine();
  if (ImGui::ArrowButton("Increase zoom", ImGuiDir_Right)) {
    view.scale *= 2;
    focusBufferWindow();
  }

  float colorAreaHeight =
    ImGui::GetFrameHeightWithSpacing() + ImGui::GetFrameHeight();
  if (ImGui::Button("Swap colors", {0, colorAreaHeight})) {
    view.swapColors();
    focusBufferWindow();
  }
  ImGui::SameLine();
  {
    ImGui::BeginChild("Colors", {0, colorAreaHeight});
    auto colorA = componentToNormalized(view.canvas.getColorA());
    if (ImGui::ColorEdit4(
          "Color A", colorA.data(), ImGuiColorEditFlags_NoInputs)) {
      view.canvas | ColorA{normalizedToComponent(colorA)};
    }
    auto colorB = componentToNormalized(view.canvas.getColorB());
    if (ImGui::ColorEdit4(
          "Color B", colorB.data(), ImGuiColorEditFlags_NoInputs)) {
      view.canvas | ColorB{normalizedToComponent(colorB)};
    }
    ImGui::EndChild();
  }
  int penSize = view.canvas.getBrush().pen.w;
  if (ImGui::SliderInt("Pen size", &penSize, 1, 16)) {
    view.canvas | Pen{penSize, penSize};
  }
  Pattern pattern = view.canvas.getBrush().pattern;
  if (PatternCombo("Tile: ", &pattern)) {
    view.canvas | pattern;
    focusBufferWindow();
  }
  if (ImGui::CollapsingHeader("Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
    bool transparent = view.isTransparent();
    if (ImGui::Checkbox("Transparent", &transparent)) {
      view.setTransparent(transparent);
      focusBufferWindow();
    }
    ImGui::Checkbox("Fill selected out region", &view.fillSelectedOut);
  }
}

void
EditorApp::showPictureWindow(const std::shared_ptr<PictureBuffer>& buffer)
{
  auto& settings = viewSettings[buffer.get()];
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
      ss << "New image " << settings.fileUnamedId << "###" << buffer.get();
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
  if (focusBufferNextFrame && buffer == view.getBuffer()) {
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
      if (redraw) {
        std::swap(view.scale, settings.scale);
        std::swap(view.offset, settings.offset);
      }
      view.setBuffer(buffer);
      SDL_SetRenderTarget(renderer, settings.texture);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      auto vp = view.getViewport();
      view.setViewport({0, 0, int(canvasSz.x), int(canvasSz.y)});
      view.update(renderer);
      view.render(renderer);

      if (ImGui::IsWindowFocused()) {
        settings.scale = view.scale;
        settings.offset = view.offset;
      } else {
        std::swap(view.scale, settings.scale);
        std::swap(view.offset, settings.offset);
      }

      view.setViewport(vp);
      SDL_SetRenderTarget(renderer, nullptr);
    }
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddImage(settings.texture, canvasP0, canvasP1);
  }
  if (!stayOpen) { close(); }
  ImGui::End();
}

void
EditorApp::setupShortcuts()
{
  shortcuts.set({.key = SDLK_n, .ctrl = true},
                [&] { requestModal = "New image"; });
  shortcuts.set({.key = SDLK_o, .ctrl = true}, [&] {
    auto buffer = loadFromFileDialog("./");
    if (buffer) { appendFile(buffer); };
  });
  shortcuts.set({.key = SDLK_ESCAPE}, [&] { view.persistSelection(); });
  shortcuts.set({.key = SDLK_DELETE}, [&] { view.setSelection(nullptr); });
  shortcuts.set({.key = SDLK_c, .ctrl = true}, [&] { copy(); });
  shortcuts.set({.key = SDLK_x, .ctrl = true}, [&] { cut(); });
  shortcuts.set({.key = SDLK_v, .ctrl = true}, [&] { paste(); });
  shortcuts.set({.key = SDLK_v, .ctrl = true, .shift = true},
                [&] { pasteAsNew(); });
  shortcuts.set({.key = SDLK_w, .ctrl = true}, [&] { close(); });
  shortcuts.set({.key = SDLK_F4, .ctrl = true}, [&] { close(); });
  shortcuts.set({.key = SDLK_s, .ctrl = true}, [&] {
    if (buffers.empty() || bufferIndex < 0) return;
    if (view.getBuffer()->getFilename().empty()) {
      saveWithFileDialog(*view.getBuffer());
    } else {
      view.getBuffer()->save();
    }
  });
  shortcuts.set({.key = SDLK_s, .ctrl = true, .shift = true}, [&] {
    if (buffers.empty() || bufferIndex < 0) return;
    saveWithFileDialog(*view.getBuffer());
  });
  shortcuts.set({.key = SDLK_z, .ctrl = true}, [&] { view.undo(); });
  shortcuts.set({.key = SDLK_z, .ctrl = true, .shift = true},
                [&] { view.redo(); });

  // Swap colors
  shortcuts.set({.key = SDLK_x, .alt = true}, [&] { view.swapColors(); });

  // grid
  shortcuts.set({.key = SDLK_g, .alt = true},
                [&] { view.enableGrid(!view.isGridEnabled()); });
}

void
EditorApp::showMainMenuBar()
{
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("New")) { requestModal = "New image"; }
      if (ImGui::MenuItem("Open", "Ctrl+O")) {
        auto buffer = loadFromFileDialog("./");
        if (buffer) { appendFile(buffer); };
      }
      if (ImGui::MenuItem("Save", "Ctrl+S")) {
        if (buffers.empty() || bufferIndex < 0) return;
        if (view.getBuffer()->getFilename().empty()) {
          saveWithFileDialog(*view.getBuffer());
        } else {
          view.getBuffer()->save();
        }
      }
      if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
        if (buffers.empty() || bufferIndex < 0) return;
        saveWithFileDialog(*view.getBuffer());
      }
      if (ImGui::MenuItem("Close File", "Ctrl+F4", nullptr, !buffers.empty())) {
        close();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Exit")) { exited = true; }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Undo", "Ctrl+Z")) { view.undo(); }
      if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z")) { view.redo(); }
      ImGui::Separator();
      if (ImGui::MenuItem("Cut", "Ctrl+X")) { cut(); }
      if (ImGui::MenuItem("Copy", "Ctrl+C")) { copy(); }
      if (ImGui::MenuItem("Paste", "Ctrl+V")) { paste(); }
      if (ImGui::MenuItem("Paste as new", "Ctrl+Shift+V")) { pasteAsNew(); }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
      ImGui::Checkbox("Maximize", &showView);
      bool gridEnabled = view.isGridEnabled();
      if (ImGui::Checkbox("Show grid", &gridEnabled)) {
        view.enableGrid(gridEnabled);
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("About")) { requestModal = "About"; }
      ImGui::EndMenu();
    }
    ImGui::Dummy({50, 0});
    ImGui::Text("%s %dx%d",
                getTool(view.getToolId()).name.c_str(),
                view.state.x,
                view.state.y);
    if (view.getBuffer()) {
      ImGui::Dummy({20, 0});
      auto& b = *view.getBuffer();
      ImGui::Text("%dx%d", b.getW(), b.getH());
      ImGui::Text("%.2f%%", view.scale * 100);
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
    ImGui::Text("File: %s", view.getBuffer()->getFilename().c_str());
    ImGui::Separator();

    if (ImGui::Button("Close",
                      ImVec2(ImGui::GetContentRegionAvail().x * .45f, 0))) {
      close(true);
      if (exiting) {
        close();
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
    close();
  }
}

} // namespace pixedit
