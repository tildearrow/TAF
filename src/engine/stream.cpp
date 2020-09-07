#include "taf.h"

Stream::Stream(int minimum, int iidd): id(iidd), bufPos(0), bufSeek(0), minReq(minimum) {
}
