file(GLOB IMGUI_SRC CONFIGURE_DEPENDS *.cpp *.h)

# ImGui SDL3 binding
add_library(imgui_sdl3
        ${IMGUI_SRC}
        backends/imgui_impl_sdl3.cpp
        backends/imgui_impl_sdl3.h
        backends/imgui_impl_sdlrenderer3.cpp
        backends/imgui_impl_sdlrenderer3.h)
target_include_directories(imgui_sdl3 PUBLIC ./ backends/)
target_link_libraries(imgui_sdl3 PUBLIC SDL3pp::SDL3pp)
