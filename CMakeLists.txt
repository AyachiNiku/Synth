cmake_minimum_required(VERSION 3.15)
project(synth)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
find_package(SDL2 REQUIRED)
# find_package(libremidi REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

add_executable(synth main.cpp
        src/synth.cpp
        src/oscillator.cpp
        src/adsr.cpp
        src/app.cpp
)
target_include_directories(synth PRIVATE ${PORTAUDIO_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
target_link_libraries(synth PRIVATE ${PORTAUDIO_LIBRARIES} ${SDL2_LIBRARIES})
