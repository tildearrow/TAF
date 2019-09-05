#include "taf.h"

std::vector<string> disarmList(string list) {
  std::vector<string> ret;
  int j;
  
  for (int i=0; i<list.size();) {
    j=list.find_first_of(' ',i);
    if (j==std::string::npos) {
      j=list.size();
    }
    ret.push_back(list.substr(i,j-i));
    i=j+1;
  }
  return ret;
}
