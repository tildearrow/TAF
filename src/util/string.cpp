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

string strFormat(const char* format, ...) {
  va_list va;
  char str[32768];
  string ret;
  va_start(va,format);
  if (vsnprintf(str,32767,format,va)<0) {
    va_end(va);
    return string("");
  }
  va_end(va);
  ret=str;
  return ret;
}
