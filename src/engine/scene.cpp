#include "taf.h"

Scene::Scene(sf::RenderTarget& w): win(&w), frame(0), timeFrame(0), cmdIndex(0) {
  defTex.loadFromFile("../res/placeholder.png");
  defTex.setSmooth(true);
  size=Coords(1920,1080);
  fps.restart();
  
  audsys.init();
}

Coords Scene::mousePos() {
  return Coords(sf::Mouse::getPosition().x/(win->getSize().x/size.x),
                sf::Mouse::getPosition().y/(win->getSize().y/size.y));
}

bool Scene::procCmd(string line) {
  Command ins;
  bool stringMode;
  bool listMode;
  bool backEscape;
  bool frameNumber;
  string arg;
  
  stringMode=false; listMode=false; backEscape=false; frameNumber=true;
  
  ins.time=0;
  
  if (line[0]=='#') {
    // comment
    ins.time=-1;
    ins.args.push_back(line);
    cmdQueue.push_back(ins);
    return true;
  }
  
  for (int c: line) {
    if (backEscape) {
      arg+=(char)c;
      backEscape=false;
    } else switch (c) {
      case '[':
        if (listMode) return false;
        listMode=true;
        break;
      case ']':
        if (!listMode) return false;
        listMode=false;
        break;
      case '"':
        stringMode=!stringMode;
        break;
      case ' ':
        if (stringMode || listMode) {
          arg+=(char)c;
        } else {
          if (frameNumber) {
            frameNumber=false;
            try {
              ins.time=std::stol(arg);
              arg="";
            } catch (std::exception& e) {
              return false;
            }
          } else {
            ins.args.push_back(arg);
            arg="";
          }
        }
        break;
      default:
        arg+=(char)c;
        break;
    }
  }
  if (frameNumber) {
    return true;
  }
  if (stringMode || listMode) {
    return false;
  }
  ins.args.push_back(arg);
  
  // preloading stuff
  if (ins.args.size()>=6) {
    if (ins.args[0]=="insert" && ins.args[1]=="Sprite") {
      printf("inserting preload on %s\n",ins.args[5].c_str());
      preload.insert(ins.args[5]);
    }
  }
  
  cmdQueue.push_back(ins);
  return true;
}

Object* Scene::findByName(string name) {
  for (Object* i: obj) {
    if (i->name==name) return i;
  }
  return NULL;
}

double Scene::getOutRate() {
  return outRate;
}

void Scene::update() {
  sf::Clock procTimeC;
  while (cmdQueue[cmdIndex].time==-1 || timeFrame==cmdQueue[cmdIndex].time) {
    if (cmdQueue[cmdIndex].time==-1) {
      cmdIndex++;
      continue;
    }
    printf("Popping at frame %ld.\n",cmdQueue[cmdIndex].time);
    Command c=cmdQueue[cmdIndex++];
    // process command
    if (c.args[0]=="identify") {
      // check size
      if (c.args.size()==3) {
        animName=c.args[1];
        animAuthor=c.args[2];
      } else {
        printf("invalid size!\n");
      }
    } else if (c.args[0]=="canvas") {
      // check size
      if (c.args.size()==3) {
        try {
          size.x=std::stoi(c.args[1]);
          size.y=std::stoi(c.args[2]);
        } catch (std::exception& e) {
          printf("invalid args!\n");
        }
      } else {
        printf("invalid size!\n");
      }
    } else if (c.args[0]=="rate") {
      // check size
      if (c.args.size()==3) {
        try {
          rate=std::stod(c.args[1]);
          outRate=std::stod(c.args[2]);
        } catch (std::exception& e) {
          printf("invalid args!\n");
        }
      } else {
        printf("invalid size!\n");
      }
    } else if (c.args[0]=="length") {
      // check size
      if (c.args.size()==3) {
        try {
          animBegin=std::stol(c.args[1]);
          animBegin=std::stol(c.args[2]);
        } catch (std::exception& e) {
          printf("invalid args!\n");
        }
      } else {
        printf("invalid size!\n");
      }
    } else if (c.args[0]=="insert") {
      if (c.args.size()>=5) {
        Coords objectPos;
        Object* created;
        try {
          objectPos=Coords(std::stod(c.args[3]),std::stod(c.args[4]));
        } catch (std::exception& e) {
          objectPos=Coords(0,0);
        }
        if (c.args[1]=="Sprite") {
          if (c.args.size()>=6) {
            created=addObject<Sprite>(objectPos,c.args[2]);
            created->setProp("file",c.args[5]);
          } else {
            printf("missing arguments!\n");
          }
        } else if (c.args[1]=="MotionSprite") {
          if (c.args.size()>=8) {
            created=addObject<MotionSprite>(objectPos,c.args[2]);
            created->setProp("file",c.args[5]);
            created->setProp("begin",c.args[6]);
            created->setProp("end",c.args[7]);
          } else {
            printf("missing arguments!\n");
          }
        } else if (c.args[1]=="Text") {
          if (c.args.size()>=7) {
            created=addObject<Text>(objectPos,c.args[2]);
            created->setProp("font",c.args[5]);
            created->setProp("fontSize",c.args[6]);
          } else {
            printf("missing arguments!\n");
          }
        } else if (c.args[1]=="Rotoscope") {
          addObject<Rotoscope>(objectPos,c.args[2]);
        } else if (c.args[1]=="PartSys") {
          addObject<PartSys>(objectPos,c.args[2]);
        } else if (c.args[1]=="AudioTrack") {
          if (c.args.size()>=6) {
            created=addObject<AudioTrack>(objectPos,c.args[2]);
            created->setProp("file",c.args[5]);
          } else {
            printf("missing arguments!\n");
          }
        } else {
          printf("invalid object type!\n");
        } 
      } else {
        printf("invalid size!\n");
      }
    } else if (c.args[0]=="prop") {
      Object* which;
      which=findByName(c.args[1]);
      if (which==NULL) {
        printf("that object doesn't exist!\n");
      } else {
        for (size_t i=2; i<c.args.size(); i+=2) {
          which->setProp(c.args[i],c.args[i+1]);
        }
      }
    } else if (c.args[0]=="move") {
      // check size
      if (c.args.size()==4) {
        try {
          Object* which;
          which=findByName(c.args[1]);
          if (which==NULL) {
            printf("that object doesn't exist!\n");
          } else {
            which->pos=Coords(std::stod(c.args[2]),std::stod(c.args[3]));
          }
        } catch (std::exception& e) {
          printf("invalid args!\n");
        }
      } else {
        printf("invalid size!\n");
      }
    } else if (c.args[0]=="animate") {
      // check size
      if (c.args.size()==4) {
        try {
          Object* which;
          which=findByName(c.args[1]);
          if (which==NULL) {
            printf("that object doesn't exist!\n");
          } else {
            which->animate(c.args[2],c.args[3]);
          }
        } catch (std::exception& e) {
          printf("invalid args!\n");
        }
      } else {
        printf("invalid size!\n");
      }
    } else if (c.args[0]=="end") {
      exit(0);
    } else {
      printf("unknown command %s\n",c.args[0].c_str());
    }
  }
  for (size_t i=0; i<obj.size(); i++) {
    if (!obj[i]->update()) {
      delete obj[i];
      obj.erase(obj.begin()+i);
    }
  }
  procTime=procTimeC.getElapsedTime().asMicroseconds();
}

void Scene::draw() {
  string cs;
  renderTime.restart();
  win->setView(sf::View(sf::FloatRect(0,0,size.x,size.y)));
  for (auto& i: obj) {
    i->draw();
  }
  // DEBUG INFO BEGIN //
  debugString=strFormat("TAF (version " TAF_VERSION ")\n % 4.0f FPS, % 3d obj, proc % 5dµs draw % 3dµs\noutFrame %d, timeFrame %d, remaining cmds %d",round(double(1000000000/fps.getElapsedTime().asMicroseconds())/1000),obj.size(),procTime,renderTime.getElapsedTime().asMicroseconds(),frame,timeFrame,cmdQueue.size()-cmdIndex);
  fps.restart();
  // DEBUG INFO END //
  frame++;
  timeFrame=frame;
}

string Scene::objDebug() {
  string ret;
  for (Object* i: obj) {
    ret+=strFormat("%s %s (%s): [%g, %g] [%gx%g] %g°\n",i->getObjectType().c_str(),i->name.c_str(),i->getLife().c_str(),i->pos.x,i->pos.y,i->scale.x,i->scale.y,i->rot);
  }
  return ret;
}

std::vector<Rect> Scene::getAllBounds() {
  std::vector<Rect> ret;
  for (Object* i: obj) {
    ret.push_back(i->getBounds());
  }
  return ret;
}
