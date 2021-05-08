//
// Created by patch on 2020/11/27.
//

#ifndef MFPLAYER_VIDEODECODER_H
#define MFPLAYER_VIDEODECODER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/codec.h>
};

#include "Base.h"
#include <pthread.h>
#include <queue>

using namespace std;

class VideoDecoder {

public:
    VideoDecoder();
    ~VideoDecoder();

    bool initDecoder(AVStream *stream, queue<RenderData> *renderQueue);
    void updateState(PlayerState newState);
    PlayerState getState();
    void start();
    void resume();
    void pause();
    void stop();

private:
    void* loopDecoder(void *callback);


private:
    PlayerState m_State = PREPARING;

    AVStream *m_Stream;

    pthread_t *m_Thread;
    pthread_mutex_t m_Mutex;

    queue<RenderData> *m_RenderQueue;

    AVCodec *m_Codec;
    AVCodecContext *m_CodecContext;

    int m_VideoWidth = 0;
    int m_VideoHeight = 0;
    int m_VideoPixFmt = 0;

};


#endif //MFPLAYER_VIDEODECODER_H
