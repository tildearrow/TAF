cmake_minimum_required(VERSION 3.0)

# legacy. it has always worked well.
# heck, it is not even legacy. you're just ignoring AMD here.
set(OpenGL_GL_PREFERENCE "LEGACY")
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
if (BUILD_STATIC)
  set(BUILD_SHARED_LIBS FALSE)
else (BUILD_STATIC)
  set(BUILD_SHARED_LIBS TRUE)
endif (BUILD_STATIC)
add_subdirectory(SFML)

project(TAF)
set(CMAKE_CXX_STANDARD 11)

include_directories(include sfml/include src/gui IconFontCppHeaders)

# temporarily use Homebrew to build on macOS
if (APPLE)
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
  find_library(HAVE_GL OpenGL PATHS ${CMAKE_OSX_SYSROOT}/System/Library PATH_SUFFIXES Frameworks NO_DEFAULT_PATH)
  find_library(HAVE_APPKIT AppKit PATHS ${CMAKE_OSX_SYSROOT}/System/Library PATH_SUFFIXES Frameworks NO_DEFAULT_PATH)
else (APPLE)
  find_library(HAVE_GL GL)
  find_library(HAVE_JACK jack)
  if (UNIX)
    find_library(HAVE_X11 X11)
  endif (UNIX)
endif (APPLE)

set(IMGUI_BASE
src/gui/imgui.cpp
src/gui/imgui_widgets.cpp
src/gui/imgui_stdlib.cpp
src/gui/imgui_draw.cpp
src/gui/imgui-SFML.cpp)

set(TAF_BASE
src/engine/animator.cpp
src/engine/audio.cpp
src/engine/decoder.cpp
src/engine/object.cpp
src/engine/preload.cpp
src/engine/scene.cpp
src/engine/stream.cpp

src/util/list.cpp
src/util/log.cpp
src/util/scale.cpp
src/util/string.cpp
src/util/strings.cpp
src/util/time.cpp

src/object/audioTrack.cpp
src/object/motionSprite.cpp
src/object/partSys.cpp
src/object/rotoscope.cpp
src/object/sprite.cpp
src/object/text.cpp)

set(GUI_BASE
src/run/editItem.cpp
src/run/main.cpp)

if (APPLE)
  list(APPEND GUI_BASE src/util/nsstub.m)
endif (APPLE)

add_executable(taf
${IMGUI_BASE}
${TAF_BASE}
${GUI_BASE})
target_link_libraries(taf sfml-graphics sfml-window sfml-system avcodec avformat avutil swscale sndfile ${HAVE_GL})

if (HAVE_JACK)
  target_link_libraries(taf ${HAVE_JACK})
endif (HAVE_JACK)

if (HAVE_X11)
  target_link_libraries(taf ${HAVE_X11})
  target_compile_definitions(taf PUBLIC HAVE_X11)
endif (HAVE_X11)

if (APPLE)
  target_link_libraries(taf ${HAVE_APPKIT})
endif (APPLE)

install(TARGETS taf RUNTIME DESTINATION bin)
