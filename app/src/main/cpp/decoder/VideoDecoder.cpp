//
// Created by patch on 2020/11/27.
//

#include "VideoDecoder.h"
#include "Utils.h"

VideoDecoder::VideoDecoder() {

}

VideoDecoder::~VideoDecoder() {

}


bool VideoDecoder::initDecoder(AVStream *stream, queue<RenderData> *renderQueue) {

    int ret = 0;

    m_Stream = stream;
    m_RenderQueue = renderQueue;

    // 初始化解码器
    m_Codec = avcodec_find_decoder(m_Stream->codecpar->codec_id);
    if (!m_Codec){
        logError(-1,"Failed to find decoder");
        return false;
    }

    LOGI("---- Find video decoder:%s ----",m_Codec->name);

    m_CodecContext = avcodec_alloc_context3(m_Codec);
    if(!m_CodecContext){
        logError(-1,"Failed to alloc codec context");
        return false;
    }

    ret = avcodec_parameters_to_context(m_CodecContext,m_Stream->codecpar);
    if(ret < 0){
        logError(ret,"Failed to copy param to context");
        return false;
    }


    ret = avcodec_open2(m_CodecContext,m_Codec,nullptr);
    if (ret < 0){
        logError(ret,"Failed to open codec");
        return false;
    }

    m_VideoWidth = m_CodecContext->width;
    m_VideoHeight = m_CodecContext->height;
    m_VideoPixFmt = m_CodecContext->pix_fmt;

    return true;
}

void VideoDecoder::updateState(PlayerState newState) {

}

PlayerState VideoDecoder::getState(){
    return m_State;
}

void* decoder(void* decoder) {
    auto *videoDecoder = static_cast<VideoDecoder*>(decoder);
    for (;;) {
        if (videoDecoder->getState() == STOP) break;

    }
}


void VideoDecoder::start() {
    pthread_create(m_Thread, nullptr, decoder, this);

}


void VideoDecoder::resume(){

}

void VideoDecoder::pause() {

}

void VideoDecoder::stop() {

}


