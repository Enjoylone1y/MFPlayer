//
// Created by patch on 2020/11/27.
//

#ifndef MFPLAYER_VIDEODECODER_H
#define MFPLAYER_VIDEODECODER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/codec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};

#include "Base.h"
#include <pthread.h>
#include <queue>

using namespace std;

class VideoDecoder {

public:
    VideoDecoder();
    ~VideoDecoder();

    bool initDecoder(AVFormatContext *fmt_ctx, VideoRenderParams *params, queue<RenderData*> *renderQueue);
    void updateState(PlayerState newState);
    PlayerState getState();
    void start();
    void resume();
    void pause();
    void stop();

    static void* threadFunc(void *decoderInst);

private:


    int loopDecode();
    void parseFrame();


private:
    PlayerState m_State = PREPARING;
    pthread_t m_Thread;

    pthread_mutex_t m_Mutex;

    queue<RenderData*> *m_RenderQueue;

    VideoRenderParams *m_RenderParams;

    AVFormatContext *m_FormatContext;
    int m_StreamIndex;
    AVStream *m_Stream;
    AVCodec *m_Codec;
    AVCodecContext *m_CodecContext;

    AVPacket *m_Packet;
    AVFrame *m_VideoFrame;
    AVFrame *m_RenderFrame;

    SwsContext *m_SwsContext;
};


#endif //MFPLAYER_VIDEODECODER_H
