#include "taf.h"

Rotoscope::Rotoscope(Scene* s, sf::RenderTarget& w): Object(s,w) {
  scale=Coords(1,1);
  rot=0;
}
