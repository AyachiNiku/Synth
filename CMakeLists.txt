cmake_minimum_required(VERSION 3.15)
project(synth)

set(CMAKE_CXX_STANDARD 20)

set(IMGUI_DIR extern/imgui)
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        # SDL2 + OpenGL backend
        ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

find_package(PkgConfig REQUIRED)
find_package(SDL2 REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

add_subdirectory(extern/libremidi)

add_executable(synth main.cpp
        src/synth.cpp
        src/oscillator.cpp
        src/adsr.cpp
        src/app.cpp
        src/voice.cpp
        src/delay.cpp
        src/filter.cpp
        ${IMGUI_SOURCES}
)

target_include_directories(synth PRIVATE
        ${PORTAUDIO_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

target_link_libraries(synth PRIVATE
        ${PORTAUDIO_LIBRARIES}
        ${SDL2_LIBRARIES}
        libremidi
        OpenGL
)
