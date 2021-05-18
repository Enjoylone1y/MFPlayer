//
// Created by patch on 2020/11/27.
//


#include "VideoDecoder.h"


VideoDecoder::VideoDecoder() {

}

VideoDecoder::~VideoDecoder() {

}


bool VideoDecoder::initDecoder(AVFormatContext *fmt_ctx, VideoRenderParams *params) {

    int ret = 0;

    m_FormatContext = fmt_ctx;

    m_RenderParams = params;

    ret = m_StreamIndex = av_find_best_stream(m_FormatContext, AVMEDIA_TYPE_VIDEO,
                                              -1, -1, nullptr, 0);
    if (ret < 0) {
        logError(ret, "Failed to find any MEDIA_TYPE_VIDEO stream");
        return false;
    }

    LOGI("---- Find video stream index:%d ----", m_StreamIndex);

    m_Stream = m_FormatContext->streams[m_StreamIndex];


    // 初始化解码器
    m_Codec = avcodec_find_decoder(m_Stream->codecpar->codec_id);
    if (!m_Codec) {
        logError(-1, "Failed to find decoder");
        return false;
    }

    LOGI("---- Find video decoder:%s ----", m_Codec->name);

    m_CodecContext = avcodec_alloc_context3(m_Codec);
    if (!m_CodecContext) {
        logError(-1, "Failed to alloc codec context");
        return false;
    }

    ret = avcodec_parameters_to_context(m_CodecContext, m_Stream->codecpar);
    if (ret < 0) {
        logError(ret, "Failed to copy param to context");
        return false;
    }

    ret = avcodec_open2(m_CodecContext, m_Codec, nullptr);
    if (ret < 0) {
        logError(ret, "Failed to open codec");
        return false;
    }

    m_TimeBase =  av_q2d(m_Stream->time_base);

    LOGI("-- videoTimeBase %lf", m_TimeBase );

    m_Packet = av_packet_alloc();
    m_VideoFrame = av_frame_alloc();

    /*视频专属*/

    // 渲染帧
    m_RenderFrame = av_frame_alloc();

    if (m_VideoFrame == nullptr || m_RenderFrame == nullptr) {
        logError(-1, "Failed to alloc frame");
        return false;
    }

    // 计算渲染帧大小
    int bufferSize = av_image_get_buffer_size(m_RenderParams->renderFormat,
            m_RenderParams->width, m_RenderParams->height, 1);

    // 分配内存
    auto *buffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));

    // 设定指针位置
    av_image_fill_arrays(m_RenderFrame->data, m_RenderFrame->linesize, buffer, m_RenderParams->renderFormat,
                         m_RenderParams->width, m_RenderParams->height, 1);

    if (m_RenderParams->renderFormat != m_CodecContext->pix_fmt){
        // 初始化转换器上下文，用于将原始视频帧转为渲染用的帧（大小和像素格式）
        m_SwsContext = sws_getContext(m_CodecContext->width, m_CodecContext->height, m_CodecContext->pix_fmt,m_RenderParams->width,
                m_RenderParams->height, m_RenderParams->renderFormat, SWS_FAST_BILINEAR,
                                      nullptr, nullptr, nullptr);
        if (!m_SwsContext) {
            logError(-1, "Failed allow sws context");
            return false;
        }
    }

    return true;
}


void VideoDecoder::updateState(PlayerState newState) {
    m_State = newState;
}


PlayerState VideoDecoder::getState() {
    return m_State;
}


void VideoDecoder::setVideoRender(VideoRender *videoRender) {
    m_VideoRender = videoRender;
}


void * VideoDecoder::threadFunc(void *decoderInst) {
    auto *videoDecoder = static_cast<VideoDecoder *>(decoderInst);
    if (videoDecoder) {
        videoDecoder->loopDecode();
    }
    return nullptr;
}

void VideoDecoder::start() {
    if (!m_Thread) {
        pthread_create(&m_Thread, nullptr, threadFunc, this);
    }
}


int VideoDecoder::loopDecode() {
    int ret;
    do {

        if(m_StartTimeStamp == -1)
            m_StartTimeStamp = getSysCurrentTime();

        ret = av_read_frame(m_FormatContext, m_Packet);
        if (ret == 0 && m_Packet->stream_index == m_StreamIndex) {
            // 解码
            ret = avcodec_send_packet(m_CodecContext, m_Packet);
            while (ret == 0) {
                ret = avcodec_receive_frame(m_CodecContext, m_VideoFrame);
                if (ret == 0) {
                    LOGI("receive videoFrame format:%d,width:%d,height:%d  lineSize:[%d,%d,%d]",
                         m_VideoFrame->format,m_VideoFrame->width, m_VideoFrame->height, m_VideoFrame->linesize[0],
                         m_VideoFrame->linesize[1], m_VideoFrame->linesize[2]);
                    parseFrame();
                } else if (ret != AVERROR_EOF && ret != AVERROR(EAGAIN)) {
                    logError(ret, "Error on decoding");
                }
            }
        }

        av_packet_unref(m_Packet);
    } while (m_State != STOP);

    return ret;
}


void VideoDecoder::updateTimeStamp() {

    if(m_VideoFrame->pkt_dts != AV_NOPTS_VALUE) {
        m_CurrentTimeStamp = m_VideoFrame->pkt_dts;
    } else if (m_VideoFrame->pts != AV_NOPTS_VALUE) {
        m_CurrentTimeStamp = m_VideoFrame->pts;
    } else {
        m_CurrentTimeStamp = 0;
    }

    m_CurrentTimeStamp = (int64_t)(m_CurrentTimeStamp * m_TimeBase * 1000);
}


long VideoDecoder::avTimeSync() {
    long curSysTime = getSysCurrentTime();
    //基于系统时钟计算从开始播放流逝的时间
    long elapsedTime = curSysTime - m_StartTimeStamp;

    //向系统时钟同步
    if(m_CurrentTimeStamp > elapsedTime) {
        //休眠时间
        auto sleepTime = static_cast<unsigned int>(m_CurrentTimeStamp - elapsedTime);//ms
        //限制休眠时间不能过长
//        sleepTime = sleepTime > DELAY_THRESHOLD ? DELAY_THRESHOLD :  sleepTime;
        av_usleep(sleepTime * 1000);
    }
    return 0;
}


void VideoDecoder::parseFrame() {

//    updateTimeStamp();
//    avTimeSync();

    switch(m_RenderParams->renderFormat){
        case AV_PIX_FMT_RGBA:{
            sws_scale(m_SwsContext, m_VideoFrame->data, m_VideoFrame->linesize,
                      0, m_VideoFrame->height, m_RenderFrame->data, m_RenderFrame->linesize);

            auto *renderData = new RenderData ();
            renderData->data[0] = m_RenderFrame->data[0];

            renderData->pts = m_VideoFrame->pts;
            renderData->key_frame = m_VideoFrame->key_frame;

            renderData->format = m_RenderParams->renderFormat;
            renderData->width = m_RenderParams->width;
            renderData->height = m_RenderParams->height;

            // 拷贝数据
            renderData->linesize[0] = m_RenderParams->width * 4;

            renderData->data[0] = (uint8_t*) malloc(renderData->linesize[0] * renderData->height);
            memcpy(renderData->data[0],m_RenderFrame->data[0],renderData->linesize[0] * renderData->height);

            m_VideoRender->renderVideoFrame(renderData);
            break;
        }

        case AV_PIX_FMT_YUV420P :{

            break;
        }

        default:
            break;
    }
}





void VideoDecoder::resume() {

}

void VideoDecoder::pause() {

}

void VideoDecoder::stop() {

}


