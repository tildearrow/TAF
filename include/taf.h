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

#define TAF_VERSION "dev12"

#define TAF_MOTION_SAMPLES 8
#define TAF_AUDIO_CHAN 2

#define S(x) std::string(x)

typedef std::string string;

extern int dw, dh;

string parentDir(string what);
string strFormat(const char* format, ...);
string getAVError(int err);
std::vector<string> disarmList(string list);

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

struct Command {
  long int time;
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

  std::vector<Object*> obj;
  
  std::queue<Command> cmdQueue;
  
  public:
    Audio audsys;
    sf::Texture defTex;
    string debugString;
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

    double getOutRate();
    
    void update();
    void draw();
    Scene(sf::RenderTarget& w);
};
