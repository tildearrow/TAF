#include "taf.h"

Text::Text(Scene* s, sf::RenderTarget& w): Object(s,w) {
  scale=Coords(1,1);
  rot=0;
}

string Text::getObjectType() {
  return "Text";
}

// TODO: finish the rest of props
bool Text::setProp(string prop, string value) {
  std::vector<string> olist;
  Object::setProp(prop,value);
  try {
    if (prop=="font") {
      if (font.loadFromFile(value)) {
        textInst.setFont(font);
        return true;
      } else {
        return false;
      }
    } else if (prop=="text") {
      text=value;
      textInst.setString(sf::String::fromUtf8(text.begin(),text.end()));
    } else if (prop=="textColor") {
      olist=disarmList(value);
      blend.r=std::stof(olist[0]);
      blend.g=std::stof(olist[1]);
      blend.b=std::stof(olist[2]);
      blend.a=std::stof(olist[3]);
      textInst.setFillColor(sf::Color(blend.r*255,blend.g*255,blend.b*255,blend.a*255));
      return true;
    } else if (prop=="fontSize") {
      fontSize=std::stod(value);
      textInst.setCharacterSize(fontSize);
      ((sf::Texture&)(font.getTexture(fontSize))).setSmooth(false);
      return true;
    } else if (prop=="lineHeight") {
      lineHeight=std::stod(value);
      textInst.setLineSpacing(lineHeight);
      return true;
    } else if (prop=="charSep") {
      charSep=std::stod(value);
      textInst.setLetterSpacing(charSep);
      return true;
    } else if (prop=="outline") {
      outline=std::stod(value);
      textInst.setOutlineThickness(outline);
      return true;
    } else if (prop=="outlineColor") {
      olist=disarmList(value);
      outColor.r=std::stof(olist[0]);
      outColor.g=std::stof(olist[1]);
      outColor.b=std::stof(olist[2]);
      outColor.a=std::stof(olist[3]);
      textInst.setOutlineColor(sf::Color(outColor.r*255,outColor.g*255,outColor.b*255,outColor.a*255));
      return true;
    }
  } catch (std::exception& e) {
    return false;
  }
  return false;
}

bool Text::update() {
  if (!Object::update()) return false;
  return true;
}

void Text::draw() {
  textInst.setPosition(pos.x,pos.y);
  textInst.setScale(scale.x,scale.y);
  textInst.setRotation(rot);
  textInst.setOrigin(orig.x*textInst.getLocalBounds().width,orig.y*textInst.getLocalBounds().height);
  win->draw(textInst);

  if (rot>360) rot-=360;
  oldPos=pos; oldScale=scale; oldRot=rot;
}
