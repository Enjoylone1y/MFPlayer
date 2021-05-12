//
// Created by patch on 2020/11/13.
//

#include "MFPlayer.h"


bool MFPlayer::init(const char *path, JNIEnv *env, jobject surface) {

    int ret = 0;

    // 打开媒体文件，找到视频流
    m_FormatContext = avformat_alloc_context();

    ret = avformat_open_input(&m_FormatContext,path,nullptr,nullptr);
    if (ret < 0){
        logError(ret,"Failed to open input");
        return false;
    }

    ret = avformat_find_stream_info(m_FormatContext,nullptr);
    if ( ret < 0){
        logError(ret,"File do not contain any stream");
        this->destroy();
        return false;
    }

    m_VideoSteamIndex = av_find_best_stream(m_FormatContext,AVMEDIA_TYPE_VIDEO,
            -1,-1,nullptr,0);

    if(m_VideoSteamIndex < 0 ){
        logError(ret,"Failed to find any MEDIA_TYPE_VIDEO stream");
    } else {
        LOGI("---- Find video stream index:%d ----",m_VideoSteamIndex);

        m_RenderQueue = new queue<RenderData*>();

        AVCodecParameters *parameters = m_FormatContext->streams[m_VideoSteamIndex]->codecpar;
        render = new ANativeWindowRender();
        if (!render->initRender(env,surface,parameters->width,parameters->height,m_RenderQueue) ){
            return false;
        }
        VideoRenderParams *params = render->getRenderParams();

        videoDecoder = new VideoDecoder();
        if (!videoDecoder->initDecoder(m_FormatContext,params,m_RenderQueue)){
            return false;
        }
    }

    m_AudioSteamIndex = av_find_best_stream(m_FormatContext,AVMEDIA_TYPE_AUDIO,
                                            -1,-1,nullptr,0);

    if(m_AudioSteamIndex < 0 ){
        logError(ret,"Failed to find any AVMEDIA_TYPE_AUDIO stream");
    } else {
        LOGI("---- Find audio stream index:%d ----",m_AudioSteamIndex);
    }

    return true;
}


void MFPlayer::play() {
    videoDecoder->start();
    render->start();
}


void MFPlayer::destroy() {

    if (m_FormatContext){
        avformat_close_input(&m_FormatContext);
    }
}


