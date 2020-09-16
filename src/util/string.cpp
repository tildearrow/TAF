#include "taf.h"

// get parent directory
string parentDir(string what) {
#ifdef _WIN32
  const char* delim="\\";
#else
  const char* delim="/";
#endif
  size_t pos=what.find_last_of(delim);
  if (pos==string::npos) return S(delim);
  return what.substr(0,pos);
}

// horrible beautify-ing
string getAVError(int err) {
  char str[32768];
  string ret;
  av_strerror(err,str,32767);
  ret=str;
  return ret;
}

// convert to timestamp
string mkTimeStamp(long int frame, double rate, bool drop) {
  if (((int)rate)==0) {
    return fmt::sprintf("??:??:??.??");
  }
  return fmt::sprintf("%02d:%02d:%02d.%02d",(frame/(int)rate)/3600,((frame/(int)rate)/60)%60,(frame/(int)rate)%60,frame%(int)rate);
}
