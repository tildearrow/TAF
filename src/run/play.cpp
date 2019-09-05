#include "taf.h"

bool quit, paused, frameAdvance, bounds;

sf::RenderWindow w;
sf::RenderTexture out;
sf::Sprite outS;
sf::Event e;

sf::Font debugFont;
sf::Text debugText;
sf::Text guideLineText;
sf::Text boundsText;

string debugStr;

Scene* s;
FILE* f;
char str[4096];
int curLine;

sf::Clock stabClock;

int dw, dh;

double scale;

int main(int argc, char** argv) {
  paused=false;
  bounds=true;
  frameAdvance=false;
  if (argc<2) {
    logE("usage: %s project\n",argv[0]);
    return 1;
  }
  
  f=fopen(argv[1],"r");
  if (f==NULL) {
    logE("error while opening file");
    return 1;
  }
  fgets(str,4095,f);
  if (strcmp(str,"---TAF PROJECT BEGIN---\n")!=0) {
    logE("not a TAF project!\n");
    return 1;
  }

  sf::VideoMode vm;
  vm=sf::VideoMode::getDesktopMode();
  dw=vm.width;
  dh=vm.height;
  
  w.create(sf::VideoMode(dw,dh),"TAF"
          ,sf::Style::Close|sf::Style::Fullscreen);
  
  out.create(1920,1080);
  outS.setTexture(out.getTexture());
  scale=double(dw)/1920.0;
  outS.setScale(scale,scale);
  out.clear();
  out.display();
  
  debugFont.loadFromFile("../res/font.ttf");
  debugText.setFont(debugFont);
  debugText.setCharacterSize((18*dw)/1920);
  debugText.setFillColor(sf::Color::White);
  debugText.setOutlineColor(sf::Color::Black);
  debugText.setOutlineThickness(1);
  debugText.setPosition(16*dw/1920,16*dw/1920);
  
  guideLineText.setFont(debugFont);
  guideLineText.setCharacterSize((12*dw)/1920);
  guideLineText.setFillColor(sf::Color::White);
  guideLineText.setOutlineColor(sf::Color::Black);
  guideLineText.setOutlineThickness(1);
  
  boundsText.setFont(debugFont);
  boundsText.setCharacterSize((12*dw)/1920);
  boundsText.setFillColor(sf::Color::Red);
  
  w.setVerticalSyncEnabled(true);

  s=new Scene(out);

#ifdef _WIN32
  SetCurrentDirectory(parentDir(argv[1]).c_str());
#else
  chdir(parentDir(argv[1]).c_str());
#endif
  
  curLine=1;
  while (!feof(f)) {
    fgets(str,4095,f);
    if (str[strlen(str)-1]=='\n') str[strlen(str)-1]=0; // strip newline char
    if (!s->procCmd(str)) {
      logE("error at line %d!\n",curLine);
      logE("> %s\n",str);
      return 1;
    }
    curLine++;
  }
  fclose(f);
  
  stabClock.restart();
  
  while (stabClock.getElapsedTime().asMilliseconds()<50) {
    w.clear();
    w.display();
  }
  
  while (1) {
    while (w.pollEvent(e)) {
      switch (e.type) {
        case sf::Event::Closed:
          quit=true;
          break;
        case sf::Event::KeyPressed:
          if (e.key.code==sf::Keyboard::Escape) {
            quit=true;
          } else if (e.key.code==sf::Keyboard::Space) {
            paused=!paused;
          } else if (e.key.code==sf::Keyboard::Period) {
            frameAdvance=true;
          } else if (e.key.code==sf::Keyboard::Tab) {
            w.setVerticalSyncEnabled(false);
          }
          break;
        case sf::Event::KeyReleased:
          if (e.key.code==sf::Keyboard::Tab) {
            w.setVerticalSyncEnabled(true);
          }
        default:
          break;
      }
    }
    // GRAPHICS CODE BEGIN //
    if (!paused || frameAdvance) {
      out.clear();
      s->update();
      s->draw();
      out.display();
      frameAdvance=false;
    }
    w.clear();
    w.draw(outS,sf::BlendNone);
    
    if (bounds) {
      sf::RectangleShape boundRect;
      boundRect.setFillColor(sf::Color::Transparent);
      boundRect.setOutlineColor(sf::Color::Red);
      boundRect.setOutlineThickness(1);
      for (Rect& i: s->getAllBounds()) {
        boundRect.setSize(sf::Vector2f(i.w*scale,i.h*scale));
        boundRect.setPosition(sf::Vector2f(i.x*scale,i.y*scale));
        w.draw(boundRect);
        boundsText.setString(strFormat("%g, %g",i.x,i.y));
        boundsText.setPosition((scale*i.x)+8,(scale*i.y)+4);
        w.draw(boundsText);
        boundsText.setString(strFormat("%g, %g",i.x+i.w,i.y+i.h));
        boundsText.setPosition((scale*(i.x+i.w))-8-boundsText.getLocalBounds().width,(scale*(i.y+i.h))-32);
        w.draw(boundsText);
      }
    }
    
    debugStr=s->debugString;
    if (paused) {
      sf::VertexArray guideLines(sf::Lines,4);
      guideLines[0].position=sf::Vector2f(0,sf::Mouse::getPosition().y);
      guideLines[1].position=sf::Vector2f(w.getSize().x,sf::Mouse::getPosition().y);
      guideLines[2].position=sf::Vector2f(sf::Mouse::getPosition().x,0);
      guideLines[3].position=sf::Vector2f(sf::Mouse::getPosition().x,w.getSize().y);
      debugStr+="\npaused.";
      w.draw(guideLines);
      guideLineText.setString(strFormat("%g, %g",(double)sf::Mouse::getPosition().x/scale,(double)sf::Mouse::getPosition().y/scale));
      guideLineText.setPosition(sf::Mouse::getPosition().x+4,sf::Mouse::getPosition().y-8-guideLineText.getLocalBounds().height);
      w.draw(guideLineText);
      //debugStr+=strFormat(" %d, %d\n",sf::Mouse::getPosition().x,sf::Mouse::getPosition().y);
    }
    debugStr+="\n\n"+s->objDebug();
    debugText.setString(sf::String::fromUtf8(debugStr.begin(),debugStr.end()));
    w.draw(debugText);
    w.display();
    // GRAPHICS CODE END //
    if (quit) break;
  }
  return 0;
}
