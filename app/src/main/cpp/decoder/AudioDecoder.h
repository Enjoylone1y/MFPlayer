//
// Created by patch on 2020/11/27.
//

#ifndef MFPLAYER_AUDIODECODER_H
#define MFPLAYER_AUDIODECODER_H

#include "Base.h"
#include "AudioRender.h"

using namespace std;

class AudioDecoder {

public:
    AudioDecoder();
    ~AudioDecoder();

    bool initDecoder(AVFormatContext *fmt_ctx, AudioRenderParams *params);

    void setAudioRender(AudioRender* audioRender);

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
    pthread_t m_Thread = 0;

    AudioRender* m_AudioRender = nullptr;

    AudioRenderParams *m_RenderParams = nullptr;

    AVFormatContext *m_FormatContext = nullptr;

    AVStream *m_Stream = nullptr;
    AVCodec *m_Codec = nullptr;
    AVCodecContext *m_CodecContext = nullptr;

    SwrContext *m_SwrContext = nullptr;

    AVPacket *m_Packet = nullptr;
    AVFrame *m_AudioFrame = nullptr;

    int m_StreamIndex;

    int m_nbSamples;
    int m_FrameDataSize;

    uint8_t *m_AudioOutBuffer;
};


#endif //MFPLAYER_AUDIODECODER_H
