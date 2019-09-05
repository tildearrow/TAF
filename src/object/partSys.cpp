#include "taf.h"

PartSys::PartSys(Scene* s, sf::RenderTarget& w): Object(s,w) {
  scale=Coords(1,1);
  rot=0;
}
