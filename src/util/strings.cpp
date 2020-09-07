#include "taf.h"

const char* cmdNames[cmdMax]={
  "noop",
  "identify",
  "canvas",
  "rate",
  "length",
  "rem",
  "insert",
  "prop",
  "move",
  "animate",
  "pipeline",
  "effect",
  "attach",
  "end"
};

const char* objTypes[objMax]={
  "Sprite",
  "MotionSprite",
  "Text",
  "Rotoscope",
  "PartSys",
  "AudioTrack",
  
  "Private",
  "Object",
  "Particle"
};
