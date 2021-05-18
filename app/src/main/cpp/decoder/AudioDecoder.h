//
// Created by patch on 2020/11/27.
//

#ifndef MFPLAYER_AUDIODECODER_H
#define MFPLAYER_AUDIODECODER_H

#include "BaseDecoder.h"
#include "AudioRender.h"

using namespace std;

class AudioDecoder : public BaseDecoder {

public:
    AudioDecoder(AVMediaType mediaType);
    ~AudioDecoder();

    bool initAVDecoder();

    void parseFrame(AVFrame* frame);

    void setAudioRender(AudioRender* audioRender);

private:

    AudioRender* m_AudioRender = nullptr;
    SwrContext *m_SwrContext = nullptr;

    int m_nbSamples;
    int m_FrameDataSize;

    uint8_t *m_AudioOutBuffer;

};


#endif //MFPLAYER_AUDIODECODER_H
