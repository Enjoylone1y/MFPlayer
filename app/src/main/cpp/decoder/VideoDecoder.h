//
// Created by patch on 2020/11/27.
//

#ifndef MFPLAYER_VIDEODECODER_H
#define MFPLAYER_VIDEODECODER_H


#include "VideoRender.h"
#include "BaseDecoder.h"

using namespace std;

class VideoDecoder : public BaseDecoder {

public:
    VideoDecoder(AVMediaType mediaType);
    ~VideoDecoder();

    bool initAVDecoder();

    void parseFrame(AVFrame* frame);

    void setVideoRender(VideoRender* videoRender);

private:

    VideoRender *m_VideoRender = nullptr;
    AVFrame *m_RenderFrame = nullptr;
    SwsContext *m_SwsContext = nullptr;
};


#endif //MFPLAYER_VIDEODECODER_H
