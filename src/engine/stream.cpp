#include "taf.h"

Stream::Stream(int minimum, int iidd): minReq(minimum), id(iidd), bufPos(0), bufSeek(0) {
}
