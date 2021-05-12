//
// Created by patch on 2020/11/27.
//

#ifndef MFPLAYER_VIDEODECODER_H
#define MFPLAYER_VIDEODECODER_H

#include "Base.h"


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
    pthread_t m_Thread = 0;

    pthread_mutex_t m_Mutex;

    queue<RenderData*> *m_RenderQueue = nullptr;

    VideoRenderParams *m_RenderParams = nullptr;

    AVFormatContext *m_FormatContext = nullptr;

    AVStream *m_Stream = nullptr;
    AVCodec *m_Codec = nullptr;
    AVCodecContext *m_CodecContext = nullptr;

    AVPacket *m_Packet = nullptr;
    AVFrame *m_VideoFrame = nullptr;
    AVFrame *m_RenderFrame = nullptr;

    SwsContext *m_SwsContext = nullptr;

    int m_StreamIndex;
};


#endif //MFPLAYER_VIDEODECODER_H
