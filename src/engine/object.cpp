#include "taf.h"

Object::Object(Scene* s, sf::RenderTarget& w): super(s), win(&w), life(0) {
  scale=Coords(0.5,0.5);
  rot=0;
  oldRot=0;
  spr.setTexture(super->defTex);
  spr.setOrigin(super->defTex.getSize().x/2,super->defTex.getSize().y/2);
}

Object::~Object() {

}

Rect Object::getBounds() {
  return Rect(spr.getGlobalBounds());
}

string Object::getObjectType() {
  return "Object";
}

string Object::getLife() {
  if (life==0) return "perennial";
  return std::to_string(life);
}

bool Object::animate(string prop, string keyframes) {
  Animator* a;
  a=new Animator;
  
  a->propName=prop;
  if (a->loadAnim(keyframes)) {
    anim.push_back(a);
    return true;
  }
  
  delete a;
  return false;
}

bool Object::setProp(string prop, string value) {
  std::vector<string> olist;
  printf("setting property %s.\n",prop.c_str());
  try {
    if (prop=="pos") {
      olist=disarmList(value);
      pos.x=std::stod(olist[0]);
      pos.y=std::stod(olist[1]);
      oldPos=pos;
      return true;
    } else if (prop=="pos.0") {
      pos.x=std::stod(value);
      return true;
    } else if (prop=="pos.1") {
      pos.y=std::stod(value);
      return true;
    } else if (prop=="scale.0") {
      scale.x=std::stod(value);
      return true;
    } else if (prop=="scale.1") {
      scale.y=std::stod(value);
      return true;
    } else if (prop=="scale") {
      olist=disarmList(value);
      scale.x=std::stod(olist[0]);
      scale.y=std::stod(olist[1]);
      oldScale=scale;
      return true;
    } else if (prop=="origin") {
      olist=disarmList(value);
      orig.x=std::stod(olist[0]);
      orig.y=std::stod(olist[1]);
      return true;
    } else if (prop=="rot") {
      rot=std::stod(value);
      oldRot=rot;
      return true;
    } else if (prop=="blend") {
      olist=disarmList(value);
      blend.r=std::stof(olist[0]);
      blend.g=std::stof(olist[0]);
      blend.b=std::stof(olist[0]);
      blend.a=std::stof(olist[0]);
      return true;
    } else if (prop=="life") {
      life=std::stol(value);
      return true;
    }
  } catch (std::exception& e) {
    return false;
  }
  return false;
}

bool Object::update() {
  for (int i=0; i<anim.size(); i++) {
    //printf("Animating Here. %s, %f = %f\n",anim[i]->propName.c_str(),anim[i]->curPos,anim[i]->getValue(anim[i]->curPos));
    if (!setProp(anim[i]->propName,std::to_string(anim[i]->getValue(anim[i]->curPos)))) {
      printf("Animation Error!\n");
    }
    anim[i]->curPos+=1;
    if (anim[i]->curPos>anim[i]->length()) {
      printf("End Of Animation\n");
      delete anim[i];
      anim.erase(anim.begin()+i);
      i--;
    }
  }
  if (life>0) {
    life--;
    if (life<1) {
      return false;
    }
  }
  return true;
}

void Object::draw() {
  // very basic and unoptimal implementation of motion blur.
  // yes, in this animation framework, motion blur is essential.
  int iCount=1+fabs(rot-oldRot)+sqrt(pow(pos.x-oldPos.x,2)+pow(pos.y-oldPos.y,2))/4;
  if (iCount>48) iCount=48;

  spr.setColor(sf::Color(255,255,255,255/iCount));
  for (int i=1; i<=iCount; i++) {
    spr.setPosition(oldPos.x+(pos.x-oldPos.x)*((float)i/(float)iCount),
                    oldPos.y+(pos.y-oldPos.y)*((float)i/(float)iCount));
    spr.setScale(oldScale.x+(scale.x-oldScale.x)*((float)i/(float)iCount),
                 oldScale.y+(scale.y-oldScale.y)*((float)i/(float)iCount));
    spr.setRotation(oldRot+(rot-oldRot)*((float)i/(float)iCount));
    win->draw(spr,sf::BlendAdd);
  }
  spr.setColor(sf::Color(255,255,255,255));

  if (rot>360) rot-=360;
  oldPos=pos; oldScale=scale; oldRot=rot;
}
