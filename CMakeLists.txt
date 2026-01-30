cmake_minimum_required(VERSION 3.22)
project(pixeditor VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)

# pixedit
file(GLOB PIXEDIT_SOURCES CONFIGURE_DEPENDS src/editorApp/*.cpp src/editorApp/*.hpp)
add_executable(pixedit ${PIXEDIT_SOURCES})
target_link_libraries(pixedit PRIVATE pix imgui tinyfiledialogs)

# libpix
file(GLOB PIX_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.hpp
    src/clipboards/*.cpp
    src/clipboards/*.hpp
    src/primitives/*.hpp
    src/tools/*.hpp
    src/utils/*.cpp
    src/utils/*.hpp
)
add_library(pix ${PIX_SOURCES})
target_link_libraries(pix PUBLIC SDL2::SDL2 SDL2_image::SDL2_image)
target_include_directories(pix 
    PUBLIC src/ 
    PRIVATE ${PROJECT_BINARY_DIR}
)

# Configuration
set(PIXEDIT_INITIAL_FILENAME "" CACHE FILEPATH "The initial filename, use \"\" to empty file")
configure_file(src/config.h.in pixedit_config.h)

# SDL2 dependency
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

# pixtest
file(GLOB_RECURSE PIXTEST_SOURCES CONFIGURE_DEPENDS test/*.cpp)
add_executable(pixtest ${PIXTEST_SOURCES})
target_include_directories(pixtest PUBLIC test/)
target_link_libraries(pixtest PRIVATE pix)

# ImGui
file(GLOB IMGUI_SOURCES external/imgui/*.cpp external/imgui/*.h)
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC external/imgui/)
target_link_libraries(imgui PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)

# tinyfiledialogs
add_library(tinyfiledialogs external/tinyfiledialogs/tinyfiledialogs.c external/tinyfiledialogs/tinyfiledialogs.h)
target_include_directories(tinyfiledialogs PUBLIC external/tinyfiledialogs/)

file(GLOB SCRATCH_SOURCES CONFIGURE_DEPENDS scratch/*.cpp)
foreach(SCRATCH_SRC ${SCRATCH_SOURCES})
    get_filename_component(SCRATCH_APP ${SCRATCH_SRC} NAME_WE)
    message(${SCRATCH_APP})
    add_executable(${SCRATCH_APP} ${SCRATCH_SRC})
    target_link_libraries(${SCRATCH_APP} PRIVATE pix)
endforeach(SCRATCH_SRC ${SCRATCH_SOURCES})
