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


    ret = avformat_open_input(&m_FormatContext,path,nullptr,nullptr);
    if (ret < 0){
        logError(ret,"Failed to open input");
        return false;
    }

    if ((ret = avformat_find_stream_info(m_FormatContext,nullptr))< 0){
        logError(ret,"File do not contain any stream");
        this->destroy();
        return false;
    }

    ret = m_StreamIndex = av_find_best_stream(m_FormatContext,AVMEDIA_TYPE_VIDEO,
            -1,-1,nullptr,0);
    if(ret < 0 ){
        logError(ret,"Failed to find any VIDEO stream");
        this->destroy();
        return false;
    }

    LOGI("---- Find video stream index:%d ----",m_StreamIndex);

    m_Stream = m_FormatContext->streams[m_StreamIndex];


    // 初始化解码器
    m_Codec = avcodec_find_decoder(m_Stream->codecpar->codec_id);
    if (!m_Codec){
        logError(-1,"Failed to find decoder");
        this->destroy();
        return false;
    }

    LOGI("---- Find video decoder:%s ----",m_Codec->name);

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


    ret = avcodec_open2(m_CodecContext,m_Codec,nullptr);
    if (ret < 0){
        logError(ret,"Failed to open codec");
        this->destroy();
        return false;
    }


    m_VideoWidth = m_CodecContext->width;
    m_VideoHeight = m_CodecContext->height;
    m_VideoPixFmt = m_CodecContext->pix_fmt;

    //初始化NativeWindow
    m_NativeWindow = ANativeWindow_fromSurface(env,surface);
    if (!m_NativeWindow){
        logError(-1,"Failed allow native window");
        this->destroy();
        return false;
    }

    // 获取窗口大小，计算最终渲染大小
    int windowWidth = ANativeWindow_getWidth(m_NativeWindow);
    int windowHeight = ANativeWindow_getHeight(m_NativeWindow);

    m_RenderWidth = m_VideoWidth;
    m_RenderHeight = m_VideoHeight;

    int result =  ANativeWindow_setBuffersGeometry(m_NativeWindow,m_RenderWidth,m_RenderHeight,
            WINDOW_FORMAT_RGBA_8888);
    if (result < 0){
        logError(-1,"Failed to set window geometry");
        this->destroy();
        return false;
    }

    m_Packet = av_packet_alloc();

    // 视频帧
    m_VideoFrame = av_frame_alloc();

    // 渲染帧
    m_RenderFrame = av_frame_alloc();

    if (m_VideoFrame == nullptr || m_RenderFrame == nullptr){
        logError(-1,"Failed to alloc frame");
        this->destroy();
        return false;
    }

    // 计算渲染帧大小
    int bufferSize = av_image_get_buffer_size(RENDER_FORMAT, m_RenderWidth, m_RenderHeight, 1);

    LOGI("render width:%d,height:%d,bufferSize:%d",m_RenderWidth,m_RenderHeight,bufferSize);

    // 分配内存
    auto buffer = (uint8_t*) av_malloc(bufferSize * sizeof(uint8_t));
    // 设定指针位置
    av_image_fill_arrays(m_RenderFrame->data, m_RenderFrame->linesize, buffer, RENDER_FORMAT, m_RenderWidth, m_RenderHeight, 1);

    // 初始化转换器上下文，用于将原始视频帧转为渲染用的帧（大小和像素格式）
    m_SwsContext = sws_getContext(m_VideoWidth, m_VideoHeight, m_VideoPixFmt, m_RenderWidth, m_RenderHeight, RENDER_FORMAT, SWS_FAST_BILINEAR,
                                  nullptr, nullptr, nullptr);
    if(!m_SwsContext){
        logError(-1,"Failed allow sws context");
        this->destroy();
    }

    return true;
}

void NativeWindowPlayer::play() {

    LOGI("---- start to play video ----");
    while (av_read_frame(m_FormatContext,m_Packet) == 0){
        if (m_Packet->stream_index == m_StreamIndex){
            LOGI("---- Got a video stream packet ----");
            this->decoderFrameAndPlay();
        }
        av_packet_unref(m_Packet);
    }

    this->destroy();
}


int NativeWindowPlayer::decoderFrameAndPlay() {

    LOGI("---- decoder a packet ----");

    int ret = 0;

    ret = avcodec_send_packet(m_CodecContext,m_Packet);
    if (ret != 0){
        logError(ret,"Error on decoding packet");
        return ret;
    }


    while (ret >= 0){
        ret = avcodec_receive_frame(m_CodecContext, m_VideoFrame);
        if (ret < 0){
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)){
                return 0;
            }
            logError(ret,"Error on decoding");
            return ret;
        }

        LOGI("receive videoFrame width:%d,height:%d  lineSize:[%d,%d,%d]",
                m_VideoFrame->width,m_VideoFrame->height, m_VideoFrame->linesize[0],
                m_VideoFrame->linesize[1], m_VideoFrame->linesize[2]);

        // 锁定缓冲区，拷贝数据
        int result = ANativeWindow_lock(m_NativeWindow,&m_WindowBuffer, nullptr);
        if (result < 0){
            logError(-1,"Failed to lock window");
            return result;
        }

        // 对每个frame做转换
        sws_scale(m_SwsContext, m_VideoFrame->data, m_VideoFrame->linesize,
                  0, m_VideoHeight, m_RenderFrame->data, m_RenderFrame->linesize);

        LOGI("AfterSws RenderFrame  width:%d,height:%d lineSize:[%d,%d,%d]",
                m_RenderFrame->width,m_RenderFrame->height, m_RenderFrame->linesize[0],
                m_RenderFrame->linesize[1], m_RenderFrame->linesize[2]);

        //获取window buffer
        auto windowBuffer = (uint8_t *)m_WindowBuffer.bits;

        // 获取输入数据的步长
        int renderLineSize = m_RenderFrame->linesize[0];
        // window 缓冲区步长
        int bufferLineSize = m_WindowBuffer.stride * 4;

        // 逐行拷贝数据
        for (int i = 0; i < m_RenderHeight; ++i) {
            memcpy(windowBuffer + i * bufferLineSize, m_RenderFrame + i * renderLineSize, renderLineSize);
        }

        // 释放缓冲区，刷新页面
        ANativeWindow_unlockAndPost(m_NativeWindow);
    }

    return 0;
}

void NativeWindowPlayer::destroy() {
    if (m_CodecContext){
        avcodec_close(m_CodecContext);
    }

    if (m_FormatContext){
        avformat_close_input(&m_FormatContext);
    }

    if (m_NativeWindow){
        ANativeWindow_release(m_NativeWindow);
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


