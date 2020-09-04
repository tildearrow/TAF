#include "taf.h"

AudioTrack::AudioTrack(Scene* s, sf::RenderTarget& w): Object(s,w), file(NULL), eof(false) {
  scale=Coords(1,1);
  rot=0;
  id=super->audsys.newStream(2048);
  memset(&info,0,sizeof(SF_INFO));
}

string AudioTrack::getObjectType() {
  return "AudioTrack";
}

Rect AudioTrack::getBounds() {
  return Rect(-512,-512,0,0);
}

bool AudioTrack::setProp(string prop, string value) {
  std::vector<string> olist;
  if (Object::setProp(prop,value)) return true;
  try {
    if (prop=="file") {
      if ((file=sf_open(value.c_str(),SFM_READ,&info))!=NULL) {
        if (sf_error(file)!=SF_ERR_NO_ERROR) {
          sf_close(file);
          return false;
        }
        return true;
      } else {
        return false;
      }
    } else if (prop=="crop1") {
    } else if (prop=="crop2") {
    }
  } catch (std::exception& e) {
    return false;
  }
  return false;
}

bool AudioTrack::update() {
  if (!Object::update()) return false;
  if (eof) return false;
  size_t feedCount;
  // play
  if (file!=NULL) {
    feedCount=sf_readf_float(file,loadBuf,735);
    if (feedCount>0) {
      super->audsys.feed(id,loadBuf,feedCount*info.channels);
    } else {
      eof=true;
    }
  }
  return true;
}

void AudioTrack::draw() {
  // TODO: fix
}
