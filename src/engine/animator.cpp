#include "taf.h"

Animator::Animator(): curPos(0) {
}

bool Animator::loadAnim(string values) {
  std::vector<string> disarmed;
  Keyframe push;
  disarmed=disarmList(values);
  try {
    for (int i=0; i<disarmed.size(); i+=3) {
      push.x=std::stod(disarmed[i]);
      push.y=std::stod(disarmed[i+1]);
      if (disarmed[i+2]=="L") {
        push.linear=true;
      } else {
        push.slope=std::stod(disarmed[i+2]);
        push.linear=false;
      }

      if (!key.empty()) {
        if (push.x<key.back().x) {
          key.clear();
          return false;
        }
      }

      key.push_back(push);
    }
  } catch (std::exception& e) {
    key.clear();
    return false;
  }
  if (key.size()<2) {
    key.clear();
    return false;
  }
  return true;
}

double Animator::length() {
  return key.back().x;
}

// Glorious 1D Cubic Bézier Formula
double Animator::getValue(double pos) {
  double a, b, c, d, x;
  if (pos<key.front().x) return key.front().y;
  for (int i=0; i<key.size(); i++) {
    if (key[i].x>pos) {
      i--;
      a=key[i].y;
      if (i==0) {
        b=a-key[i].slope/3; // inverted left slope
      } else {
        b=a+key[i].slope/3; // right slope
      }
      d=key[i+1].y;
      c=d+key[i+1].slope/3; // left slope
      
      x=(pos-key[i].x)/(key[i+1].x-key[i].x);
      
      //printf("A: %f B: %f C: %f D: %f X: %f\n",a,b,c,d,x);
      
      return a*pow(1-x,3)+3*b*pow(1-x,2)*x+3*c*(1-x)*pow(x,2)+d*pow(x,3);
    }
  }
  return key.back().y;
}
