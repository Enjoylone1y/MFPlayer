//
// Created by A on 2021/5/18.
//

#ifndef MFPLAYER_BASEDECODER_H
#define MFPLAYER_BASEDECODER_H

#include "Base.h"

class BaseDecoder {

public:
    bool initDecoder(AVFormatContext *fmt_ctx, RenderParams *params);

    void updateState(PlayerState newState);
    PlayerState getState();
    void start();
    void resume();
    void pause();
    void stop();

    static void * threadFunc(void *decoderInst);

    virtual bool initAVDecoder() = 0;

    virtual void parseFrame(AVFrame* frame) = 0;

protected:
    PlayerState m_State = PREPARING;

    pthread_t m_Thread = 0;
    pthread_mutex_t m_Mutex;

    RenderParams *m_RenderParams = nullptr;

    AVFormatContext *m_FormatContext = nullptr;

    AVMediaType m_MediaType;

    AVStream *m_Stream = nullptr;
    AVCodec *m_Codec = nullptr;
    AVCodecContext *m_CodecContext = nullptr;

    AVPacket *m_Packet = nullptr;
    AVFrame *m_Frame = nullptr;

    int m_StreamIndex;


private:

    double m_TimeBase = 0;

    int64_t m_CurrentTimeStamp = 0;
    int64_t m_StartTimeStamp = -1;

    //更新显示时间戳
    void updateTimeStamp();
    //音视频同步
    long avTimeSync();

    int loopDecode();

};




#endif //MFPLAYER_BASEDECODER_H
