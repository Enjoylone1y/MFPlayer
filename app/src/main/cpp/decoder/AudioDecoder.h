//
// Created by patch on 2020/11/27.
//

#ifndef MFPLAYER_AUDIODECODER_H
#define MFPLAYER_AUDIODECODER_H

#include "Base.h"

using namespace std;

class AudioDecoder {
public:
    AudioDecoder();
    ~AudioDecoder();

    bool initDecoder(AVFormatContext *fmt_ctx, AudioRenderParams *params, queue<RenderData*> *renderQueue);
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

    PlayerState m_State = PREPARING;
    pthread_t m_Thread;

    pthread_mutex_t m_Mutex;

    queue<RenderData*> *m_RenderQueue;

    AudioRenderParams *m_RenderParams;

    AVFormatContext *m_FormatContext;
    int m_StreamIndex;
    AVStream *m_Stream;
    AVCodec *m_Codec;
    AVCodecContext *m_CodecContext;

    SwrContext *m_SwrContext;

    AVPacket *m_Packet;
    AVFrame *m_AudioFrame;

    int m_nbSamples;
    int m_FrameDataSize;

    uint8_t *m_AudioOutBuffer;
};


#endif //MFPLAYER_AUDIODECODER_H
