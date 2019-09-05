#include "taf.h"

#define refcount 0

bool VideoDecoder::open(string name, string path) {
  int ret;
  AVCodec* decInfo=NULL;
  AVDictionary* decOptions=NULL;
  if ((ret=avformat_open_input(&format,path.c_str(),NULL,NULL))<0) {
    logW("%s: could not open file! %d\n",name.c_str(),getAVError(ret).c_str());
    return false;
  }
  
  if (avformat_find_stream_info(format,NULL)<0) {
    logW("%s: invalid file contents!\n",name.c_str());
    return false;
  }
  
  
  if ((index=av_find_best_stream(format,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0))<0) {
    logW("%s: this file doesn't contain video!\n",name.c_str());
    return false;
  }
  strm = format->streams[index];
  
  if ((decInfo=avcodec_find_decoder(strm->codecpar->codec_id))==NULL) {
      logW("%s: can't decode this video format!\n",name.c_str());
      return false;
  }
  
  if ((decoder=avcodec_alloc_context3(decInfo))==NULL) {
      logW("%s: out of memory for decoder!\n",name.c_str());
      return false;
  }
  
  if ((ret=avcodec_parameters_to_context(decoder,strm->codecpar))<0) {
      logW("%s: unable to copy codec parameters!\n",name.c_str());
      return false;
  }
  
  if (refcount) {
    av_dict_set(&decOptions, "refcounted_frames","1",0);
  } else {
    av_dict_set(&decOptions, "refcounted_frames","0",0);
  }
  decoder->thread_count=4;
  decoder->thread_type=FF_THREAD_FRAME;

  if (avcodec_open2(decoder,decInfo,&decOptions)<0) {
      logW("%s: can't create decoder!\n",name.c_str());
      return false;
  }

  width=decoder->width;
  height=decoder->height;
  pixFormat=decoder->pix_fmt;
  copySize=av_image_alloc(convData,convPitch,width,height,AV_PIX_FMT_RGBA,1);
  if (copySize<0) {
    logW("%s: unable to create conversion image!\n",name.c_str());
    return false;
  }

  frame=av_frame_alloc();
  convFrame=av_frame_alloc();
  if (!frame || !convFrame) {
    logW("%s: couldn't create my frame(s)!\n",name.c_str());
    return false;
  }

  av_init_packet(&packet);
  packet.size=0;
  packet.data=NULL;
  
  if ((converter=sws_getContext(width,height,pixFormat,width,height,AV_PIX_FMT_RGBA,SWS_BICUBIC,NULL,NULL,NULL))==NULL) {
    logW("%s: couldn't create pixel format converter!\n",name.c_str());
    return false;
  }

  opened=true;
  return true;
}

bool VideoDecoder::isOpen() {
  return opened;
}

bool VideoDecoder::decode() {
  int len, prevLen, hasFrame;
  hasFrame=0;
  
  len=-1;
  
  while (av_read_frame(format,&packet)>=0) {
    AVPacket orig_pkt=packet;
    do {
      prevLen=packet.size;

      if (packet.stream_index==index) {
        if (avcodec_send_packet(decoder,&packet)<0) {
          printf("couldn't decode frame...\n");
          break;
        } else {
          len=avcodec_receive_frame(decoder,frame);
          if (len==-EAGAIN || len==AVERROR_EOF) {
            hasFrame=-1;
          } else {
            hasFrame=1;
          }

          if (hasFrame>=0) {
            // check if the frame is the same
            if (frame->width!=width || frame->height!=height || frame->format!=pixFormat) {
              logW("format has changed!\n");
              break;
            }
          
            // transform
            sws_scale(converter,frame->data,frame->linesize,0,frame->height,convData,convPitch);

            // copy frame pointer
            frameData=convData[0];
            frameTime.tv_sec=(frame->pts*strm->time_base.num)/strm->time_base.den;
            frameTime.tv_nsec=((frame->pts*strm->time_base.num)%strm->time_base.den)*((1000000000LL*strm->time_base.num)/strm->time_base.den);
          }
          len=prevLen;
        }
      }

      // free data if required
      if (hasFrame && refcount) {
        av_frame_unref(frame);
      }
      
      if (len<0) break;
      packet.data+=len;
      packet.size-=len;
    } while (packet.size>0);
    av_packet_unref(&orig_pkt);
    if (hasFrame>0) break;
  }
  if (len<0) return false;
  return true;
}

// TODO: I-frame lookup
// however this can be implemented later
bool VideoDecoder::seek(struct timespec time) {
  long int convTime;

  convTime=(time.tv_sec*strm->time_base.den+((time.tv_nsec/1000)*strm->time_base.den)/1000000)/strm->time_base.num;
  av_seek_frame(format,index,convTime,AVSEEK_FLAG_BACKWARD);
  return true;
}

VideoDecoder::VideoDecoder(): format(NULL), decoder(NULL), decodeCount(0), strm(NULL), frame(NULL), convFrame(NULL), converter(NULL), opened(false), frameData(NULL), frameTime(mkts(0,0)) {
  for (int i=0; i<4; i++) {
    convData[i]=NULL;
    convPitch[i]=0;
  }
}

VideoDecoder::~VideoDecoder() {
  // cleanup. not complete as it somehow leaks memory.
  avcodec_free_context(&decoder);
  avformat_close_input(&format);
  sws_freeContext(converter);
  av_freep(&convData[0]);
  av_frame_free(&frame);
  av_frame_free(&convFrame);
}
