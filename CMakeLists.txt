cmake_minimum_required(VERSION 3.22)
project(pixeditor VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)

# pixedit
file(GLOB PIXEDIT_SOURCES CONFIGURE_DEPENDS src/editorApp/*.cpp src/editorApp/*.hpp)
add_executable(pixedit ${PIXEDIT_SOURCES})
target_link_libraries(pixedit PRIVATE pix imgui_sdl3 tinyfiledialogs nlohmann_json::nlohmann_json SQLiteCpp)

# libpix
file(GLOB PIX_SOURCES CONFIGURE_DEPENDS
        src/*.cpp
        src/*.hpp
        src/clipboards/*.cpp
        src/clipboards/*.hpp
        src/primitives/*.hpp
        src/tools/*.hpp
)
add_library(pix
        src/utils/Color.hpp
        src/utils/cutoutSurface.hpp
        src/utils/dumpSurface.hpp
        src/utils/paths.cpp
        src/utils/paths.hpp
        src/utils/pixel.hpp
        src/utils/rasterLine.hpp
        src/utils/rasterOval.hpp
        src/utils/rasterPoly.hpp
        src/utils/rect.hpp
        src/utils/renderSelection.hpp
        src/utils/replayPicture.cpp
        src/utils/replayPicture.hpp
        src/utils/safeGetFormat.hpp
        src/utils/TempSurface.cpp
        src/utils/TempSurface.hpp
        ${PIX_SOURCES}
)
target_link_libraries(pix PUBLIC SDL3pp::SDL3pp)
target_include_directories(pix
        PUBLIC src/
        PRIVATE ${PROJECT_BINARY_DIR}
)

# Configuration
set(PIXEDIT_INITIAL_FILENAME "" CACHE FILEPATH "The initial filename, use \"\" to empty file")
configure_file(src/config.h.in pixedit_config.h)

# pixtest
file(GLOB_RECURSE PIXTEST_SOURCES CONFIGURE_DEPENDS test/*.cpp)
add_executable(pixtest ${PIXTEST_SOURCES})
target_include_directories(pixtest PUBLIC test/)
target_link_libraries(pixtest PRIVATE pix)

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
