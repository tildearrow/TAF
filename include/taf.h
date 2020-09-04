#ifndef _TAF_H
#define _TAF_H
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <string>
#include <vector>
#include <queue>
#include <SFML/Graphics.hpp>
#include <sndfile.h>
#include <jack/jack.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "ta-log.h"
#include "ta-time.h"

extern "C" {
  #include <libavutil/imgutils.h>
  #include <libavutil/samplefmt.h>
  #include <libavutil/timestamp.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

#define TAF_VERSION "dev14"

#define TAF_MOTION_SAMPLES 8
#define TAF_AUDIO_CHAN 2

#define S(x) std::string(x)

typedef std::string string;

extern int dw, dh;

string parentDir(string what);
string strFormat(const char* format, ...);
string getAVError(int err);
std::vector<string> disarmList(string list);

enum Commands {
  cmdNoOp=0,
  cmdIdentify,
  cmdCanvas,
  cmdRate,
  cmdLength,
  cmdRem,
  cmdInsert,
  cmdProp,
  cmdMove,
  cmdAnimate,
  cmdPipeline,
  cmdEffect,
  cmdAttach,
  cmdEnd,
  
  cmdMax
};

// about properties:
// - they can have any type in PropertyFlags
// - they can be either value, or array
// - if array, its members are accessed using prop.INDEX
// - INDEX may also be any of the GLSL vector components (with a custom rect extension):
//   - prop.0  .x  .r  .s  .x
//   - prop.1  .y  .g  .t  .y
//   - prop.2  .z  .b  .p  .W
//   - prop.3  .w  .a  .q  .H

enum Properties {
  propPos=0,
  propScale,
  propOrigin,
  propRot,
  propBlend,
  propLife,

  // for file-based objects
  propFile,
  propCrop,

  // for motion objects
  propSeek,
  propSpeed,

  // for text
  propFont,
  propText,
  propTextColor,
  propFontSize,
  propLineHeight,
  propCharSep,
  propOutline,
  propOutlineColor,

  propMax
};

enum PropertyFlags {
  pfChar=0,
  pfShort,
  pfInt,
  pfLong,

  pfHalf,
  pfFloat,
  pfDouble,

  pfString,

  pfReadOnly=0x100,
  pfSetCall=0x200,
};

extern const char* cmdNames[cmdMax];
extern const char* propNames[propMax];

struct Color {
  float r, g, b, a;
  Color(sf::Color c): r(c.r*255), g(c.g*255), b(c.b*255), a(c.a*255) {}
  Color(float red, float green, float blue, float alpha): r(red), g(green), b(blue), a(alpha) {}
  Color(float red, float green, float blue): r(red), g(green), b(blue), a(1) {}
  Color(): r(1), g(1), b(1), a(1) {}
};

struct Coords {
  double x, y;
  Coords(double xpos, double ypos): x(xpos), y(ypos) {}
  Coords(): x(0), y(0) {}
};

struct Rect {
  double x, y, w, h;
  Rect(sf::FloatRect r): x(r.left), y(r.top), w(r.width), h(r.height) {}
  Rect(double xpos, double ypos, double width, double height): x(xpos), y(ypos), w(width), h(height) {}
  Rect(): x(0), y(0), w(0), h(0) {}
};

struct Keyframe {
  // right slope if first point, left slope otherwise
  // right slope is assumed to be the inverse of the left one
  double x, y, slope;
  bool linear;
};

// forward
class Scene;

class VideoDecoder {
  AVFormatContext* format;
  AVCodecContext* decoder;
  unsigned char* convData[4];
  int convPitch[4];
  int decodeCount;
  AVPixelFormat pixFormat;
  AVStream* strm;
  AVFrame* frame;
  AVFrame* convFrame;
  AVPacket packet;
  SwsContext* converter;
  bool opened;
  public:
    int width, height, index, copySize;
    unsigned char* frameData;
    struct timespec frameTime;
    bool seek(struct timespec time);
    double getPosTime();
    long int getPosFrame();
    bool decode();
    bool isOpen();
    bool open(string name, string path);
    VideoDecoder();
    ~VideoDecoder();
};

struct PreloadImage {
  sf::Image* img;
  string path;
  PreloadImage(string p): path(p) {}
};

class PreloadPool {
  std::queue<string> pqueue;
  std::vector<PreloadImage> pool;
  sf::Thread* thr;
  sf::Mutex poolLock;
  bool quit;
  public:
    void runJob();
    sf::Image* acquire(string path);
    void insert(string path);
    PreloadPool();
    ~PreloadPool();
};

class Animator {
  std::vector<Keyframe> key;
  public:
    string propName;
    double curPos;
    double getValue(double pos);
    double length();
    bool loadAnim(string values);
    Animator();
};

class Object {
  protected:
    Scene* super;
    sf::RenderTarget* win;
    sf::Sprite spr;

    double oldRot;
    sf::Transform samples[TAF_MOTION_SAMPLES];
    
    Color blend;
    long int life;
    std::vector<Animator*> anim;
  public:
    string name;
    Coords pos, scale, orig;
    Coords oldPos, oldScale, oldOrig;
    double rot;
    virtual string getObjectType();
    string getLife();
    virtual Rect getBounds();
    virtual bool setProp(string prop, string value);
    bool animate(string prop, string keyframes);
    virtual bool update();
    virtual void draw();
    Object(Scene* s, sf::RenderTarget& w);
    virtual ~Object();
};

class Sprite: public Object {
  sf::Texture tex;
  Coords crop[2];
  public:
    string getObjectType();
    bool setProp(string prop, string value);
    void draw();
    Sprite(Scene* s, sf::RenderTarget& w);
};

class MotionSprite: public Object {
  sf::Texture tex;
  VideoDecoder dec;
  long int begin, end;
  double speed;
  long int curFrame;
  struct timespec trackTime;
  public:
    string getObjectType();
    bool setProp(string prop, string value);
    bool update();
    void draw();
    MotionSprite(Scene* s, sf::RenderTarget& w);
};

class Text: public Object {
  sf::Font font;
  sf::Text textInst;
  string fontPath;
  string text;
  double fontSize;
  double lineHeight;
  double charSep;
  double outline;
  Color textColor;
  Color outColor;
  public:
    string getObjectType();
    bool setProp(string prop, string value);
    bool update();
    void draw();
    Text(Scene* s, sf::RenderTarget& w);
};

class Rotoscope: public Object {
  public:
    Rotoscope(Scene* s, sf::RenderTarget& w);
};

class PartSys: public Object {
  public:
    PartSys(Scene* s, sf::RenderTarget& w);
};

class AudioTrack: public Object {
  int id;
  SNDFILE* file;
  SF_INFO info;
  float loadBuf[4096];
  bool eof;
  public:
    string getObjectType();
    Rect getBounds();
    bool setProp(string prop, string value);
    bool update();
    void draw();
    AudioTrack(Scene* s, sf::RenderTarget& w);
};

struct Stream {
  int id;
  float buf[8192][TAF_AUDIO_CHAN];
  int bufPos, bufSeek;
  int minReq;
  Stream(int minimum, int id);
};

class Audio {
  jack_client_t* ac;
  jack_port_t* ao[2];
  jack_status_t as;
  std::vector<Stream> streams;
  public:
    int process(unsigned int nframes, void* arg);
    int feed(int id, float* buf, size_t len);
    int newStream(int minimum);
    bool destroyStream(int id);
    bool init();
    Audio();
};

// NOTE: about time:
// - -1: comment
// - -2: empty line
struct Command {
  long int time;
  unsigned char cmd;
  std::vector<string> args;
};

class Scene {
  Coords size;
  sf::RenderTarget* win;
  string animName, animAuthor;
  sf::Clock fps;
  sf::Clock renderTime;
  long int procTime;
  long int frame, timeFrame;
  double rate, outRate;
  long int animBegin, animEnd;
  
  public:
    long int cmdIndex;
    std::vector<Object*> obj;
    std::vector<Command> cmdQueue;
    Audio audsys;
    sf::Texture defTex;
    string debugString;
    PreloadPool preload;
    template<typename T> Object* addObject(Coords pos, string name) {
      obj.push_back(new T(this, *win));
      obj[obj.size()-1]->pos=pos;
      obj[obj.size()-1]->oldPos=pos;
      obj[obj.size()-1]->name=name;
      return obj[obj.size()-1];
    }
    
    Coords mousePos();
    string objDebug();
    string getCmdQueue();
    std::vector<Rect> getAllBounds();
    
    Object* findByName(string name);
    
    bool procCmd(string line);
    bool procDel(int index);

    double getOutRate();
    
    bool update();
    void draw();
    Scene(sf::RenderTarget& w);
};

#endif
