//
// Created by patch on 2020/11/27.
//

#include "VideoDecoder.h"
#include "Utils.h"

VideoDecoder::VideoDecoder() {

}

VideoDecoder::~VideoDecoder() {

}


bool VideoDecoder::initDecoder(AVFormatContext *fmt_ctx, VideoRenderParams *params, queue<RenderData*> *renderQueue) {

    int ret = 0;

    m_FormatContext = fmt_ctx;
    m_RenderQueue = renderQueue;
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


    m_Packet = av_packet_alloc();

    // 视频帧
    m_VideoFrame = av_frame_alloc();

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
    auto buffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));

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


void *VideoDecoder::threadFunc(void *decoderInst) {
    auto *videoDecoder = static_cast<VideoDecoder *>(decoderInst);
    if (videoDecoder) {
        videoDecoder->loopDecode();
    }
    return nullptr;
}


int VideoDecoder::loopDecode() {
    int ret = 0;
    do {
        ret = av_read_frame(m_FormatContext, m_Packet);
        if (m_Packet->stream_index == m_StreamIndex) {
            ret = avcodec_send_packet(m_CodecContext, m_Packet);
            while (ret >= 0) {
                ret = avcodec_receive_frame(m_CodecContext, m_VideoFrame);
                if (ret < 0) {
                    if (ret != AVERROR_EOF && ret != AVERROR(EAGAIN)) {
                        logError(ret, "Error on decoding");
                    }
                } else {
                    LOGI("receive videoFrame format:%d,width:%d,height:%d  lineSize:[%d,%d,%d]",
                         m_VideoFrame->format,m_VideoFrame->width, m_VideoFrame->height, m_VideoFrame->linesize[0],
                         m_VideoFrame->linesize[1], m_VideoFrame->linesize[2]);
                    parseFrame();
                }
            }
        }
        av_packet_unref(m_Packet);
    } while (m_State != STOP);

    return ret;
}


void VideoDecoder::parseFrame() {
    switch(m_RenderParams->renderFormat){
        case AV_PIX_FMT_RGBA:{
            sws_scale(m_SwsContext, m_VideoFrame->data, m_VideoFrame->linesize,
                      0, m_VideoFrame->height, m_RenderFrame->data, m_RenderFrame->linesize);

            LOGI("AfterSws RenderFrame  format:%d,width:%d,height:%d lineSize:[%d,%d,%d]",
                 m_RenderFrame->format,m_RenderFrame->width,m_RenderFrame->height, m_RenderFrame->linesize[0],
                 m_RenderFrame->linesize[1], m_RenderFrame->linesize[2]);

            auto *renderData = new RenderData ();
            renderData->data[0] = m_RenderFrame->data[0];
            renderData->linesize[0] = m_RenderFrame->linesize[0];

            renderData->pts = m_VideoFrame->pts;

            renderData->format = m_RenderParams->renderFormat;
            renderData->width = m_RenderParams->width;
            renderData->height = m_RenderParams->height;

            m_RenderQueue->push(renderData);
            break;
        };

        default:
            break;
    }
}



void VideoDecoder::start() {
    if (!m_Thread) {
        pthread_create(&m_Thread, nullptr, threadFunc, this);
    }
}


void VideoDecoder::resume() {

}

void VideoDecoder::pause() {

}

void VideoDecoder::stop() {

}


