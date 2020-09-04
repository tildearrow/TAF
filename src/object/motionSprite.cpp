#include "taf.h"

MotionSprite::MotionSprite(Scene* s, sf::RenderTarget& w): Object(s,w), curFrame(0) {
  scale=Coords(1,1);
  rot=0;
  trackTime=mkts(0,0);
}

string MotionSprite::getObjectType() {
  return "MotionSprite";
}

bool MotionSprite::setProp(string prop, string value) {
  std::vector<string> olist;
  if (Object::setProp(prop,value)) return true;
  try {
    if (prop=="file") {
      if (dec.open(name,value)) {
        printf("opened! going for it.\n");
        tex.create(dec.width,dec.height);
        tex.setSmooth(true);
        spr.setTexture(tex,true);
        dec.decode();
        tex.update(dec.frameData);
        return true;
      } else {
        printf("not opened! placeholder instead...\n");
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

// TODO: move decoding to draw in order to allow seeking
bool MotionSprite::update() {
  if (!Object::update()) return false;
  if (dec.isOpen()) {
    trackTime=trackTime+long(1000000000/super->getOutRate());
  }
  return true;
}

void MotionSprite::draw() {
  int iCount=1+fabs(rot-oldRot)+sqrt(pow(pos.x-oldPos.x,2)+pow(pos.y-oldPos.y,2))/4;
  if (iCount>48) iCount=48;
  
  // TODO: speed up
  while (dec.frameTime<trackTime) {
    printf("decoding... times: %s<%s\n",tstos(dec.frameTime).c_str(),tstos(trackTime).c_str());
    tex.update(dec.frameData);
    curFrame++;
    if (!dec.decode()) {
      logE("error while decoding!\n");
      break;
    }
  }

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
