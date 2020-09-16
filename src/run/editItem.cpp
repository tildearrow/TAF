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

// convert argument to command argument
string argize(string a) {
  string b;
  for (auto& i: a) {
    if (i=='"' || i=='\\') {
      b+='\\';
    }
    b+=i;
  }
  if (a[0]=='[' && a[a.size()-1]==']') {
    return b;
  }
  if (a.find(" ")!=string::npos) {
    return "\""+b+"\"";
  }
  return b;
}

string EditItem::compose() {
  string ret;
  switch (cmd) {
    case cmdNoOp:
      ret=fmt::sprintf("%d noop",time);
      break;
    case cmdIdentify:
      ret=fmt::sprintf("%d identify %s %s",time,argize(id.name),argize(id.author));
      break;
    case cmdCanvas:
      ret=fmt::sprintf("%d canvas %d %d",time,this->c.width,this->c.height);
      break;
    case cmdRate:
      ret=fmt::sprintf("%d rate %g %g",time,r.sr,r.ofr);
      break;
    case cmdLength:
      ret=fmt::sprintf("%d canvas %d %d",time,l.begin,l.end);
      break;
    case cmdRem:
      ret=fmt::sprintf("%d rem %s",time,rem.text);
      break;
    case cmdInsert:
      ret=fmt::sprintf("%d insert %s %s %g %g",time,objTypes[in.type],argize(in.name),in.x,in.y);
      // TODO: type arguments
      break;
    case cmdProp:
      ret=fmt::sprintf("%d prop %s",time,argize(p.obj));
      for (Property& i: p.props) {
        ret+=fmt::sprintf(" %s %s",argize(i.name),argize(i.value));
      }
      break;    
    case cmdMove:
      ret=fmt::sprintf("%d move %s %g %g",time,argize(m.obj),m.x,m.y);
      break;
    case cmdAnimate:
      ret=fmt::sprintf("%d animate %s %s",time,argize(a.obj),argize(a.prop));
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
  return ret;
}
