#include "taf.h"

int audioCallback(unsigned int nframes, void* arg) {
  return ((Audio*)arg)->process(nframes,arg);
}

Audio::Audio(): ac(NULL) {
}

const char* chanLetters[4]={
  "L", "R", "F", "B"
};

bool Audio::init() {
  ac=jack_client_open("TAF",JackNoStartServer,&as);
  if (ac==NULL) return false;
  const char* cna=jack_get_client_name(ac);
  jack_set_process_callback(ac,audioCallback,this);

  for (int i=0; i<TAF_AUDIO_CHAN; i++) {
    ao[i]=jack_port_register(ac,(std::string("out")+chanLetters[i]).c_str(),JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
  }

  jack_activate(ac);

  for (int i=0; i<TAF_AUDIO_CHAN; i++) {
    jack_connect(ac,(std::string(cna)+":out"+chanLetters[i]).c_str(),strFormat("system:playback_%d",i+1).c_str());
  }
  return true;
}

int Audio::newStream(int minimum) {
  // for now. multi-stream support coming soon
  streams.push_back(Stream(minimum,0));
  return 0;
}

int Audio::feed(int id, float* buf, size_t len) {
  for (Stream& i: streams) {
    if (i.id==id) {
      if ((i.bufSeek+(len/2))>8191) {
        //("copying beyond border\n");
        memcpy(&i.buf[i.bufSeek],buf,sizeof(float)*2*(8191-i.bufSeek));
        memcpy(i.buf,&buf[(8191-i.bufSeek)*2],sizeof(float)*2*((i.bufSeek+(len/2))-8191));
      } else {
        //printf("copying normally\n");
        memcpy(&i.buf[i.bufSeek],buf,sizeof(float)*(len));
      }
      i.bufSeek+=len/2;
      i.bufSeek&=8191;
      return len;
    }
  }
  return -1;
}

int Audio::process(unsigned int nframes, void* arg) {
  float* stream[TAF_AUDIO_CHAN];
  for (int i=0; i<TAF_AUDIO_CHAN; i++) {
    stream[i]=(float*)jack_port_get_buffer(ao[i],nframes);
  }
  for (int i=0; i<nframes; i++) {
    for (int j=0; j<TAF_AUDIO_CHAN; j++) {
      stream[j][i]=0;
    }
    for (Stream& k: streams) {
      if (k.bufSeek!=k.bufPos) {
        for (int j=0; j<TAF_AUDIO_CHAN; j++) {
          stream[j][i]+=k.buf[k.bufPos][j];
        }
        k.bufPos++;
        k.bufPos&=8191;
      } else {
        // we ran out of samples
        //printf("OOS\n");
      }
    }
  }
  return 0;
}
