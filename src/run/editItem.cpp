#include "taf.h"

bool EditItem::decompose(Command c) {
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
      break;
    case cmdRate:
      break;
    case cmdLength:
      break;
    case cmdRem:
      break;
    case cmdInsert:
      break;
    case cmdProp:
      break;    
    case cmdMove:
      break;
    case cmdAnimate:
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
  ret="rem todo";
  return ret;
}
