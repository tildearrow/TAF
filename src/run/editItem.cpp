#include "taf.h"

bool EditItem::decompose(Command c) {
  bool insertValue=false;
  Property next;
  
  time=c.time;
  cmd=c.cmd;
  
  switch (c.cmd) {
    case cmdNoOp:
      break;
    case cmdIdentify:
      id.name=c.args[1];
      id.author=c.args[2];
      break;
    case cmdCanvas:
      this->c.width=std::stoi(c.args[1]);
      this->c.height=std::stoi(c.args[2]);
      break;
    case cmdRate:
      r.sr=std::stod(c.args[1]);
      r.ofr=std::stod(c.args[2]);
      break;
    case cmdLength:
      l.begin=std::stoi(c.args[1]);
      l.end=std::stoi(c.args[2]);
      break;
    case cmdRem:
      rem.text="";
      for (size_t i=1; i<c.args.size(); i++) {
        rem.text+=c.args[i];
        if (i!=c.args.size()-1) {
          rem.text+=' ';
        }
      }
      break;
    case cmdInsert:
      in.type=objSprite;
      for (int i=0; i<objPrivate; i++) {
        if (c.args[1]==objTypes[i]) {
          in.type=i;
          break;
        }
      }
      
      in.name=c.args[2];
      
      in.x=std::stoi(c.args[3]);
      in.y=std::stoi(c.args[4]);
      
      // TODO: type arguments
      break;
    case cmdProp:
      p.obj=c.args[1];
      p.props.clear();
      
      for (size_t i=2; i<c.args.size(); i++) {
        if (insertValue) {
          next.value=c.args[i];
          p.props.push_back(next);
        } else {
          next.name=c.args[i];
          next.value="";
        }
        insertValue=!insertValue;
      }
      break;    
    case cmdMove:
      m.obj=c.args[1];
      
      in.x=std::stoi(c.args[2]);
      in.y=std::stoi(c.args[3]);
      break;
    case cmdAnimate:
      a.obj=c.args[1];
      a.prop=c.args[2];
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
      break;
  }
  return true;
}

string EditItem::compose() {
  string ret;
  ret=strFormat("%d rem todo",time);
  return ret;
}
