cmake_minimum_required(VERSION 3.22)
project(pixeditor VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)

# pixedit
add_executable(pixedit 
        editorApp/EditorApp.cpp
        editorApp/EditorApp.hpp
        editorApp/FileDialogTinyfd.cpp
        editorApp/FileDialogTinyfd.hpp
        editorApp/ImGuiComponent.cpp
        editorApp/ImGuiComponent.hpp
        editorApp/main.cpp
        editorApp/shortcutPlugin.cpp
        editorApp/shortcutPlugin.hpp
        editorApp/ViewSettings.hpp
        editorApp/imgui/AboutDialog.hpp
        editorApp/imgui/AuxWindowManager.hpp
        editorApp/imgui/BufferSelectionWindow.hpp
        editorApp/imgui/ConfirmExitDialog.hpp
        editorApp/imgui/MainMenu.hpp
        editorApp/imgui/NewFileDialog.hpp
        editorApp/imgui/PictureOptionsWindow.hpp
        editorApp/imgui/PictureWindow.hpp
)
target_link_libraries(pixedit PRIVATE pix imgui_sdl3 tinyfiledialogs)

# libpix
add_library(pix
        src/Action.cpp
        src/Action.hpp
        src/Brush.hpp
        src/Canvas.cpp
        src/Canvas.hpp
        src/Clipboard.cpp
        src/Clipboard.hpp
        src/config.h.in
        src/defaults.cpp
        src/defaults.hpp
        src/Id.cpp
        src/Id.hpp
        src/loaders.cpp
        src/loaders.hpp
        src/MouseState.hpp
        src/Pattern.hpp
        src/Pen.hpp
        src/PictureBuffer.cpp
        src/PictureBuffer.hpp
        src/PictureFile.hpp
        src/PictureManager.cpp
        src/PictureManager.hpp
        src/PictureView.cpp
        src/PictureView.hpp
        src/PluginManager.cpp
        src/PluginManager.hpp
        src/replayPicture.cpp
        src/replayPicture.hpp
        src/savers.cpp
        src/savers.hpp
        src/ShortcutManager.hpp
        src/ToolDescription.hpp
        src/tools.cpp
        src/tools.hpp
        src/primitives/Blit.hpp
        src/primitives/Line.hpp
        src/primitives/Lines.hpp
        src/primitives/Oval.hpp
        src/primitives/Point.hpp
        src/primitives/Poly.hpp
        src/primitives/Rect.hpp
        src/tools/FloodFillTool.hpp
        src/tools/FreeHandTool.hpp
        src/tools/LinesTool.hpp
        src/tools/OvalTool.hpp
        src/tools/PolyTool.hpp
        src/tools/RectTool.hpp
        src/tools/SelectionFreeTool.hpp
        src/tools/SelectionHandTool.hpp
        src/tools/SelectionRectTool.hpp
        src/tools/ZoomTool.hpp
)
target_link_libraries(pix PUBLIC SDL3pp::SDL3pp pix_utils pix_format)
target_include_directories(pix
        PUBLIC src/
        PRIVATE ${PROJECT_BINARY_DIR}
)

# Configuration
set(PIXEDIT_INITIAL_FILENAME "" CACHE FILEPATH "The initial filename, use \"\" to empty file")
configure_file(src/config.h.in pixedit_config.h)

# pixtest
add_executable(pixtest 
        # test/CanvasFillRectangleScenario.cpp
        # test/CanvasPointScenario.cpp
        test/catch.cpp
        test/catch.hpp
        test/PluginManagerTest.cpp
        # test/utils/PixReaderTest.cpp
        # test/utils/PixWritterTest.cpp
        test/utils/rasterLineTest.cpp
        test/utils/rasterPolyTest.cpp
)
target_include_directories(pixtest PUBLIC test/)
target_link_libraries(pixtest PRIVATE pix)

# pix_utils
add_library(pix_utils OBJECT
        utils/Color.hpp
        utils/cutoutSurface.hpp
        utils/dumpSurface.hpp
        utils/paths.cpp
        utils/paths.hpp
        utils/pixel.hpp
        utils/rasterLine.hpp
        utils/rasterOval.hpp
        utils/rasterPoly.hpp
        utils/rect.hpp
        utils/renderSelection.hpp
        utils/safeGetFormat.hpp
        utils/TempSurface.cpp
        utils/TempSurface.hpp
)
target_link_libraries(pix_utils PUBLIC SDL3pp::SDL3pp)
target_include_directories(pix_utils PUBLIC utils/)

# pix_format
add_library(pix_format OBJECT
        pixformat/saveSurface.cpp
        pixformat/saveSurface.hpp
        pixformat/dbActions.hpp
        pixformat/dbActions.cpp
)
target_link_libraries(pix_format PUBLIC SDL3pp::SDL3pp nlohmann_json::nlohmann_json SQLiteCpp pix_utils)
target_include_directories(pix_format PUBLIC pixformat/)

# Deps
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SQLiteCpp CONFIG REQUIRED)

# ImGui

add_library(imgui_sdl3
        external/imgui_sdl3/imgui_impl_sdl3.cpp
        external/imgui_sdl3/imgui_impl_sdl3.h
        external/imgui_sdl3/imgui_impl_sdlrenderer3.cpp
        external/imgui_sdl3/imgui_impl_sdlrenderer3.h)
target_include_directories(imgui_sdl3 PUBLIC external/imgui_sdl3/)
target_link_libraries(imgui_sdl3 PUBLIC SDL3pp::SDL3pp imgui::imgui)

# tinyfiledialogs
add_library(tinyfiledialogs external/tinyfiledialogs/tinyfiledialogs.c external/tinyfiledialogs/tinyfiledialogs.h)
target_include_directories(tinyfiledialogs PUBLIC external/tinyfiledialogs/)

file(GLOB SCRATCH_SOURCES CONFIGURE_DEPENDS scratch/*.cpp)
foreach (SCRATCH_SRC ${SCRATCH_SOURCES})
    get_filename_component(SCRATCH_APP ${SCRATCH_SRC} NAME_WE)
    message(${SCRATCH_APP})
    add_executable(${SCRATCH_APP} ${SCRATCH_SRC})
    target_link_libraries(${SCRATCH_APP} PRIVATE pix)
endforeach (SCRATCH_SRC ${SCRATCH_SOURCES})

include(FetchContent)
set(SDL3PP_FORCE_BUNDLED ON) # Enable this to force CMake to download SDL, SDL_image and SDL_ttf
FetchContent_Declare(SDL3ppExternal
        URL https://github.com/talesm/SDL3pp/releases/download/0.7.3/SDL3pp-0.7.3.tar.gz
)
FetchContent_MakeAvailable(SDL3ppExternal)

# Link to your project
target_link_libraries(pix
        PUBLIC SDL3pp::SDL3pp
)
