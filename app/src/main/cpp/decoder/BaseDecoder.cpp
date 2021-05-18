//
// Created by A on 2021/5/18.
//

#include "BaseDecoder.h"

bool BaseDecoder::initDecoder(AVFormatContext *fmt_ctx, RenderParams *params) {

    int ret = 0;

    m_FormatContext = fmt_ctx;

    m_RenderParams = params;

    ret = m_StreamIndex = av_find_best_stream(m_FormatContext, m_MediaType,
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
    m_Frame = av_frame_alloc();

    return initAVDecoder();
}


void * BaseDecoder::threadFunc(void *decoderInst) {
    auto *videoDecoder = static_cast<BaseDecoder *>(decoderInst);
    if (videoDecoder) {
        videoDecoder->loopDecode();
    }
    return nullptr;
}

void BaseDecoder::start() {
    if (!m_Thread) {
        pthread_create(&m_Thread, nullptr, threadFunc, this);
    }
}


int BaseDecoder::loopDecode() {
    int ret;
    do {
        
        if(m_StartTimeStamp == -1)
            m_StartTimeStamp = getSysCurrentTime();

        ret = av_read_frame(m_FormatContext, m_Packet);
        if (ret == 0 && m_Packet->stream_index == m_StreamIndex) {
            // 解码
            ret = avcodec_send_packet(m_CodecContext, m_Packet);
            while (ret == 0) {
                ret = avcodec_receive_frame(m_CodecContext, m_Frame);
                if (ret == 0) {
                    updateTimeStamp();
                    avTimeSync();
                    parseFrame(m_Frame);
                } else if (ret != AVERROR_EOF && ret != AVERROR(EAGAIN)) {
                    logError(ret, "Error on decoding");
                }
            }
        }

        av_packet_unref(m_Packet);
    } while (m_State != STOP);

    return ret;
}


void BaseDecoder::updateTimeStamp() {

    if(m_Frame->pkt_dts != AV_NOPTS_VALUE) {
        m_CurrentTimeStamp = m_Frame->pkt_dts;
    } else if (m_Frame->pts != AV_NOPTS_VALUE) {
        m_CurrentTimeStamp = m_Frame->pts;
    } else {
        m_CurrentTimeStamp = 0;
    }

    m_CurrentTimeStamp = (int64_t)(m_CurrentTimeStamp * m_TimeBase * 1000);
}


long BaseDecoder::avTimeSync() {
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