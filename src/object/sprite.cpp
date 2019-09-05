#include "taf.h"

Sprite::Sprite(Scene* s, sf::RenderTarget& w): Object(s,w) {
  scale=Coords(1,1);
  rot=0;
}

string Sprite::getObjectType() {
  return "Sprite";
}

bool Sprite::setProp(string prop, string value) {
  std::vector<string> olist;
  if (Object::setProp(prop,value)) return true;
  try {
    if (prop=="file") {
      if (tex.loadFromFile(value)) {
        tex.setSmooth(true);
        spr.setTexture(tex,true);
        return true;
      } else {
        return false;
      }
    } else if (prop=="crop1") {
    } else if (prop=="crop2") {
    }
  } catch (std::exception& e) {
    return false;
  }
  return false;
}

void Sprite::draw() {
  int iCount=1+fabs(rot-oldRot)+sqrt(pow(pos.x-oldPos.x,2)+pow(pos.y-oldPos.y,2))/4;
  if (iCount>48) iCount=48;

  if (anim.empty()) {
    spr.setPosition(pos.x,pos.y);
    spr.setScale(scale.x,scale.y);
    spr.setRotation(rot);
    spr.setOrigin(orig.x,orig.y);
    win->draw(spr);
    oldPos=pos; oldScale=scale; oldRot=rot;
  } else {
    spr.setColor(sf::Color(255,255,255,255/iCount));
    spr.setOrigin(orig.x,orig.y);
    for (int i=1; i<=iCount; i++) {
      spr.setPosition(oldPos.x+(pos.x-oldPos.x)*((float)i/(float)iCount),
                      oldPos.y+(pos.y-oldPos.y)*((float)i/(float)iCount));
      spr.setScale(oldScale.x+(scale.x-oldScale.x)*((float)i/(float)iCount),
                   oldScale.y+(scale.y-oldScale.y)*((float)i/(float)iCount));
      spr.setRotation(oldRot+(rot-oldRot)*((float)i/(float)iCount));
      win->draw(spr,sf::BlendAdd);
    }
    spr.setColor(sf::Color(255,255,255,255));

    oldPos=pos; oldScale=scale; oldRot=rot;
  }
}
