cmake_minimum_required(VERSION 3.0)

project(TAF)

include_directories(include)

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
src/util/string.cpp
src/util/time.cpp

src/object/audioTrack.cpp
src/object/motionSprite.cpp
src/object/partSys.cpp
src/object/rotoscope.cpp
src/object/sprite.cpp
src/object/text.cpp)

add_executable(taf-play
${TAF_BASE}
src/run/play.cpp)

target_link_libraries(taf-play sfml-graphics sfml-window sfml-system avcodec avformat avutil swscale jack sndfile)

install(TARGETS taf-play RUNTIME DESTINATION bin)
