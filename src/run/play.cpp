#include "taf.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include <IconsFontAwesome4.h>

bool quit, playing, frameAdvance, bounds;

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

namespace ImGui {
  void Image(const sf::RenderTexture& texture, const sf::Vector2f& size, const sf::FloatRect& textureRect, const sf::Color& tintColor, const sf::Color& borderColor) {
    sf::Vector2f textureSize = static_cast<sf::Vector2f>(texture.getSize());
    ImVec2 uv0(textureRect.left / textureSize.x, (textureRect.top + textureRect.height) /   textureSize.y);
    ImVec2 uv1((textureRect.left + textureRect.width) / textureSize.x, textureRect.top / textureSize.y);
    Image(texture.getTexture().getNativeHandle(), size, uv0, uv1, tintColor, borderColor);
  }
}

void analyzeCmd(Command c, int index) {
  if (c.time==-1) {
    ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.5,1,1,1));
    ImGui::Selectable(c.args[0].c_str());
  } else if (c.args.empty()) {
    ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1,0.5,0.5,1));
    ImGui::Selectable("???");
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1,1,0.5,1));
    ImGui::Selectable(c.args[0].c_str());
  }
  ImGui::PopStyleColor();
  bool status=ImGui::IsItemHovered();
  
  if (status) {
    ImGui::SameLine();
    ImGui::SmallButton(ICON_FA_PENCIL);
    if (ImGui::IsItemHovered()) {
      if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        printf("edit item!\n");
      }
    }
    ImGui::SameLine();
    ImGui::SmallButton(ICON_FA_TIMES);
    if (ImGui::IsItemHovered()) {
      if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        printf("delete item!\n");
        s->procDel(index);
      }
    }
  }
  
  for (int i=1; i<c.args.size(); i++) {
    ImGui::SameLine();
    ImGui::Text("%s",c.args[i].c_str());
  }
}

int main(int argc, char** argv) {
  playing=false;
  bounds=true;
  frameAdvance=false;
  if (argc<2) {
    logE("usage: %s project\n",argv[0]);
    return 1;
  }
  
  f=fopen(argv[1],"r");
  if (f==NULL) {
    logE("error while opening file\n");
    return 1;
  }
  fgets(str,4095,f);
  if (strcmp(str,"---TAF PROJECT BEGIN---\n")!=0) {
    logE("not a TAF project!\n");
    return 1;
  }

  /*sf::VideoMode vm;
  vm=sf::VideoMode::getDesktopMode();*/
  dw=2560;
  dh=1440;
  
  w.create(sf::VideoMode(dw,dh),"TAF"
          ,sf::Style::Close|sf::Style::Resize);

  ImGui::SFML::Init(w,false);
  
  //ImGui::GetIO().Fonts->Clear();
  ImGui::GetStyle().ScaleAllSizes(2);
  
  ImGui::GetIO().Fonts->AddFontFromFileTTF("../res/font.ttf",18*2);
  
  ImFontConfig fc;
  fc.MergeMode=true;
  fc.GlyphMinAdvanceX=13.0f;
  static const ImWchar fir[]={ICON_MIN_FA,ICON_MAX_FA,0};
  ImGui::GetIO().Fonts->AddFontFromFileTTF("../res/FontAwesome.otf",18*2,&fc,fir);
  ImGui::SFML::UpdateFontTexture();
  
  out.create(1920,1080);
  outS.setTexture(out.getTexture());
  scale=double(dw)/3840.0;
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
  
  sf::Clock imClock;
  while (1) {
    while (w.pollEvent(e)) {
      ImGui::SFML::ProcessEvent(e);

      switch (e.type) {
        case sf::Event::Closed:
          quit=true;
          break;
        case sf::Event::KeyPressed:
          if (e.key.code==sf::Keyboard::Escape) {
            quit=true;
          } else if (e.key.code==sf::Keyboard::Space) {
            playing=!playing;
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
    // GUI CODE BEGIN //
    ImGui::SFML::Update(w,imClock.restart());

    ImGui::Begin("Playback",NULL,ImGuiWindowFlags_NoTitleBar);
    ImGui::Button(ICON_FA_FAST_BACKWARD);
    ImGui::SameLine();
    ImGui::Button(ICON_FA_BACKWARD);
    ImGui::SameLine();
    if (playing) {
      if (ImGui::Button(ICON_FA_PAUSE)) {
        playing=!playing;
      }
    } else {
      if (ImGui::Button(ICON_FA_PLAY)) {
        playing=!playing;
      }
    }
    ImGui::SameLine();
    ImGui::Button(ICON_FA_FORWARD);
    ImGui::SameLine();
    ImGui::Button(ICON_FA_FAST_FORWARD);
    ImGui::End();

    ImGui::Begin("Script");
    ImGui::Columns(3,NULL,false);
    ImGui::SetColumnWidth(0,48);
    ImGui::SetColumnWidth(1,96);
    for (int i=0; i<s->cmdQueue.size(); i++) {
      if ((s->cmdIndex-1)==i) {
        ImGui::Text(ICON_FA_CHEVRON_RIGHT);
      }
      ImGui::NextColumn();
      if (s->cmdQueue[i].time==-1) {
        /*
        ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.5,1,1,1));
        ImGui::Selectable("#");
        ImGui::PopStyleColor();*/
      } else {
        ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.5,1,0.5,1));
        ImGui::Selectable(strFormat("%ld",s->cmdQueue[i].time).c_str());
        ImGui::PopStyleColor();
      }
      ImGui::NextColumn();
      analyzeCmd(s->cmdQueue[i],i);
      ImGui::NextColumn();
    }
    ImGui::End();
    
    ImGui::Begin("Inspector");
    ImGui::Text("%s",s->objDebug().c_str());
    ImGui::End();

    ImGui::Begin("Viewer");
    ImGui::Image(out,sf::Vector2f(1280,720),sf::FloatRect(0,0,1920,1080),sf::Color::White,sf::Color::Transparent);
    ImGui::End();
    // GUI CODE END //

    // GRAPHICS CODE BEGIN //
    if (playing || frameAdvance) {
      if (s->update()) {
        out.clear();
        s->draw();
        out.display();
        frameAdvance=false;
      } else {
        playing=false;
      }
    }
    w.clear();
    //w.draw(outS,sf::BlendNone);
    
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
    if (!playing) {
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
    debugText.setString(sf::String::fromUtf8(debugStr.begin(),debugStr.end()));
    w.draw(debugText);

    ImGui::SFML::Render(w);

    w.display();
    // GRAPHICS CODE END //
    if (quit) break;
  }
  return 0;
}
