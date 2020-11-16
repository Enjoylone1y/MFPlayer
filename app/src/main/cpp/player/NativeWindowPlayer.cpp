//
// Created by patch on 2020/11/13.
//

#include "NativeWindowPlayer.h"

#define LOG_TAG "MFPlayer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void logError(int ret,const char* msg);

bool NativeWindowPlayer::init(const char *path,JNIEnv *env,jobject surface) {

    int ret = 0;

    // 打开媒体文件，找到视频流
    m_FormatContext = avformat_alloc_context();
    ret = avformat_open_input(&m_FormatContext,path,NULL,NULL);
    if (ret < 0){
        logError(ret,"Failed to open input");
        return false;
    }

    if ((ret = avformat_find_stream_info(m_FormatContext,NULL))< 0){
        logError(ret,"File do not contain any stream");
        this->destroy();
        return false;
    }

    ret = m_StreamIndex = av_find_best_stream(m_FormatContext,AVMEDIA_TYPE_VIDEO,
            -1,-1,NULL,0);
    if(ret <= 0 ){
        logError(ret,"Failed to find any VIDEO stream");
        this->destroy();
        return false;
    }

    m_Stream = m_FormatContext->streams[m_StreamIndex];

    // 初始化解码器
    m_Codec = avcodec_find_decoder(m_Stream->codecpar->codec_id);
    if (!m_Codec){
        logError(-1,"Failed to find codec");
        this->destroy();
        return false;
    }

    m_CodecContext = avcodec_alloc_context3(m_Codec);
    if(!m_CodecContext){
        logError(-1,"Failed to alloc codec context");
        this->destroy();
        return false;
    }

    ret = avcodec_parameters_to_context(m_CodecContext,m_Stream->codecpar);
    if(ret < 0){
        logError(ret,"Failed to copy param to context");
        this->destroy();
        return false;
    }

    ret = avcodec_open2(m_CodecContext,m_Codec,NULL);
    if (ret < 0){
        logError(ret,"Failed to open codec");
        this->destroy();
        return false;
    }

    m_VideoWidth = m_CodecContext->width;
    m_VideoHeight = m_CodecContext->height;
    m_PixFormat = m_CodecContext->pix_fmt;

    //初始化NativeWindow
    m_NativeWindow = ANativeWindow_fromSurface(env,surface);


    return true;
}

void NativeWindowPlayer::play() {

}

void NativeWindowPlayer::destroy() {
    if (m_CodecContext){
        avcodec_close(m_CodecContext);
    }
    if (m_FormatContext){
        avformat_close_input(&m_FormatContext);
    }
}


static void logError(int ret,const char* msg){

    if (ret == -1){
        LOGE("%s\n",msg);
    } else {
        char errStr[1024];
        av_strerror(ret,errStr,1024);
        LOGE("%s: %d\t%s\n",msg,ret,errStr);
    }
}


