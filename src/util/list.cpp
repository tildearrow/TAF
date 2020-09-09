#include "taf.h"
#include <stdexcept>

std::vector<string> disarmList(string list) {
  std::vector<string> ret;
  string alist=list;
  size_t j;
  
  if (alist.find('[')!=0 || alist.find(']')!=(alist.size()-1)) {
    throw std::invalid_argument("not an array");
  }
  
  alist.erase(alist.begin());
  alist.erase(alist.end()-1);
  
  for (size_t i=0; i<alist.size();) {
    j=alist.find_first_of(' ',i);
    if (j==std::string::npos) {
      j=alist.size();
    }
    ret.push_back(alist.substr(i,j-i));
    i=j+1;
  }
  return ret;
}
