#include "taf.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui-SFML.h>
#include <IconsFontAwesome4.h>

bool quit, playing, frameAdvance, bounds, shallRedraw;

sf::RenderWindow w;
sf::RenderTexture out;
sf::Event e;

string debugStr;

Scene* s;
FILE* f;
char str[4096];
int curLine;

sf::Clock stabClock;

int dw, dh;

double scale;

EditItem edit;
int editItemPos;

namespace ImGui {
  void Image(const sf::RenderTexture& texture, const sf::Vector2f& size, const sf::FloatRect& textureRect, const sf::Color& tintColor, const sf::Color& borderColor) {
    sf::Vector2f textureSize = static_cast<sf::Vector2f>(texture.getSize());
    ImVec2 uv0(textureRect.left / textureSize.x, (textureRect.top + textureRect.height) /   textureSize.y);
    ImVec2 uv1((textureRect.left + textureRect.width) / textureSize.x, textureRect.top / textureSize.y);
    Image(texture.getTexture().getNativeHandle(), size, uv0, uv1, tintColor, borderColor);
  }
}

char stringtest[4096];

void drawEditItem(int index) {
  ImGui::Combo("Command",&edit.cmd,cmdNames,cmdMax);
  
  ImGui::Separator();
  
  // the fuss
  switch (edit.cmd) {
    case cmdNoOp:
      break;
    case cmdIdentify:
      ImGui::InputText("Name",&edit.id.name);
      ImGui::InputText("Author",&edit.id.author);
      break;
    case cmdCanvas:
      ImGui::InputInt("Width",&edit.c.width);
      ImGui::InputInt("Height",&edit.c.height);
      break;
    case cmdRate:
      ImGui::InputDouble("Tick rate",&edit.r.sr);
      ImGui::InputDouble("Frame rate",&edit.r.ofr);
      break;
    case cmdLength:
      ImGui::InputInt("Begin",&edit.l.begin);
      ImGui::InputInt("End",&edit.l.end);
      break;
    case cmdRem:
      ImGui::InputText("Text",&edit.rem.text);
      break;
    case cmdInsert:
      ImGui::Combo("Type",&edit.in.type,objTypes,objPrivate);
      ImGui::InputText("Name",&edit.in.name);
      ImGui::InputFloat("X",&edit.in.x);
      ImGui::InputFloat("Y",&edit.in.y);
      
      // TODO: arguments for object types
      break;
    case cmdProp:
      ImGui::InputText("Object",&edit.p.obj);
      
      for (size_t i=0; i<edit.p.props.size(); i++) {
        ImGui::PushID(strFormat("prop%d",i).c_str());
        ImGui::SetNextItemWidth(192);
        ImGui::InputText(":",&edit.p.props[i].name);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(400);
        ImGui::InputText(".",&edit.p.props[i].value);
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_TIMES)) {
          edit.p.props.erase(edit.p.props.begin()+i);
          i--;
        }
        ImGui::PopID();
      }
      if (ImGui::Button(ICON_FA_PLUS)) {
        Property newprop;
        newprop.name=""; newprop.value="";
        edit.p.props.push_back(newprop);
      }
      break;    
    case cmdMove:
      ImGui::InputText("Object",&edit.m.obj);
      
      ImGui::InputFloat("X",&edit.m.x);
      ImGui::InputFloat("Y",&edit.m.y);
      break;
    case cmdAnimate:
      ImGui::InputText("Object",&edit.a.obj);
      ImGui::InputText("Property",&edit.a.prop);
      
      ImGui::Button(ICON_FA_PENCIL " Edit");
      break;
    case cmdPipeline:
      break;
    case cmdEffect:
      break;
    case cmdAttach:
      break;
    case cmdEnd:
      break;
    default:
      ImGui::Text("this command is not implemented. please bug the author.");
      break;
  }
  
  if (ImGui::Button("OK")) {
    editItemPos=-1;
    if (!s->repCmd(index,edit.compose())) {
      logE("we failed\n");
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel")) {
    editItemPos=-1;
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
        editItemPos=index;
        edit.decompose(c);
      }
    }
    ImGui::SameLine();
    ImGui::SmallButton(ICON_FA_TIMES);
    if (ImGui::IsItemHovered()) {
      if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        printf("delete item!\n");
        s->procDel(index);
        shallRedraw=true;
      }
    }
  }
  
  for (size_t i=1; i<c.args.size(); i++) {
    ImGui::SameLine();
    ImGui::Text("%s",c.args[i].c_str());
  }
  
  if (index==editItemPos) {
    drawEditItem(index);
  }
}

int main(int argc, char** argv) {
  playing=false;
  bounds=true;
  frameAdvance=false;
  shallRedraw=false;
  editItemPos=-1;
  
  if (argc<2) {
    // blank project
    f=NULL;
  } else {
    f=fopen(argv[1],"r");
    if (f==NULL) {
      logE("error while opening file\n");
      return 1;
    }
    memset(str,0,4096);
    fgets(str,4095,f);
    if (strcmp(str,"---TAF PROJECT BEGIN---\n")!=0) {
      logE("not a TAF project!\n");
      return 1;
    }
  }

  /*sf::VideoMode vm;
  vm=sf::VideoMode::getDesktopMode();*/
  dw=2560;
  dh=1440;
  
  w.create(sf::VideoMode(dw,dh),"TAF"
          ,sf::Style::Close|sf::Style::Resize);

  ImGui::SFML::Init(w,false);
  
  scale=2; //getScale();
  ImGui::GetStyle().ScaleAllSizes(scale);
  
  if (!ImGui::GetIO().Fonts->AddFontFromFileTTF("../res/font.ttf",18*scale)) {
    logE("could not load UI font!\n");
    return 1;
  }
  
  ImFontConfig fc;
  fc.MergeMode=true;
  fc.GlyphMinAdvanceX=13.0f;
  static const ImWchar fir[]={ICON_MIN_FA,ICON_MAX_FA,0};
  if (!ImGui::GetIO().Fonts->AddFontFromFileTTF("../res/FontAwesome.otf",18*2,&fc,fir)) {
    logE("could not load icons!\n");
    return 1;
  }
  ImGui::SFML::UpdateFontTexture();
  
  // TODO: set to proper canvas resolution
  if (!out.create(1920,1080)) {
    logE("could not create output!\n");
    return 1;
  }
  out.clear();
  out.display();
  
  w.setVerticalSyncEnabled(true);

  s=new Scene(out);
  if (s==NULL) {
    logE("could not create scene!\n");
    return 1;
  }
  
  if (f==NULL) {
    // load a new project
    s->procCmd("0 identify \"New Project\" \"Author\"");
    s->procCmd("0 canvas 1920 1080");
    s->procCmd("0 rate 30 30");
    s->procCmd("0 length 1 7300");
  } else {
#ifdef _WIN32
    SetCurrentDirectory(parentDir(argv[1]).c_str());
#else
    chdir(parentDir(argv[1]).c_str());
#endif
    
    curLine=1;
    // TODO: handle strings larger than 4095
    while (!feof(f)) {
      fgets(str,4095,f);
      if (strlen(str)>0) {
        if (str[strlen(str)-1]=='\n') str[strlen(str)-1]=0; // strip newline char
      }
      if (!s->procCmd(str)) {
        logE("error at line %d!\n",curLine);
        logE("> %s\n",str);
        return 1;
      }
      curLine++;
    }
    fclose(f);
  }
  
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
          if (e.key.code==sf::Keyboard::Space) {
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
    
    // menu bar
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("file")) {
      ImGui::MenuItem("new");
      ImGui::MenuItem("open...");
      ImGui::Separator();
      ImGui::MenuItem("save");
      ImGui::MenuItem("save as...");
      ImGui::Separator();
      if (ImGui::MenuItem("exit")) {
        quit=true;
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("edit")) {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("window")) {
      ImGui::MenuItem("playback");
      ImGui::MenuItem("script");
      ImGui::MenuItem("inspector");
      ImGui::MenuItem("viewer");
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    // playback controls
    ImGui::Begin("Playback",NULL,ImGuiWindowFlags_NoTitleBar);
    ImGui::Columns(3,NULL,false);
    if (ImGui::Button(ICON_FA_FAST_BACKWARD)) {
      s->seekFrame(0);
      shallRedraw=true;
    }
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
    
    ImGui::NextColumn();
    ImGui::Text("%s",mkTimeStamp(s->timeFrame,s->outRate,false).c_str());
    
    ImGui::NextColumn();
    if (ImGui::InputInt("Frame",(int*)&s->frame,1,1,ImGuiInputTextFlags_EnterReturnsTrue)) {
      playing=false;
      s->seekFrame(s->frame);
      shallRedraw=true;
    }
    ImGui::End();

    // script
    ImGui::Begin("Script");
    ImGui::Columns(3,NULL,false);
    ImGui::SetColumnWidth(0,48);
    ImGui::SetColumnWidth(1,96);
    for (size_t i=0; i<s->cmdQueue.size(); i++) {
      if ((s->cmdIndex)==i) {
        ImGui::Text(ICON_FA_CHEVRON_RIGHT);
      }
      ImGui::NextColumn();
      if (s->cmdQueue[i].time==-1) {
        ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.5,1,1,1));
        ImGui::Text("---");
        ImGui::PopStyleColor();
      } else {
        ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.5,1,0.5,1));
        if (ImGui::Selectable(strFormat("%ld",s->cmdQueue[i].time).c_str())) {
          // TODO: seek by command
          logI("seeking! %ld\n",s->cmdQueue[i].time);
          s->seekFrame(s->cmdQueue[i].time);
          shallRedraw=true;
        }
        ImGui::PopStyleColor();
      }
      ImGui::NextColumn();
      analyzeCmd(s->cmdQueue[i],i);
      ImGui::NextColumn();
    }
    ImGui::End();
    
    // inspector
    ImGui::Begin("Inspector");
    ImGui::Text("%s",s->objDebug().c_str());
    ImGui::End();

    // viewer
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
        shallRedraw=false;
      } else {
        playing=false;
      }
    }
    if (shallRedraw) {
      out.clear();
      s->draw();
      out.display();
      shallRedraw=false;
    }

    w.clear();
    ImGui::SFML::Render(w);
    w.display();
    // GRAPHICS CODE END //
    if (quit) break;
  }
  return 0;
}
