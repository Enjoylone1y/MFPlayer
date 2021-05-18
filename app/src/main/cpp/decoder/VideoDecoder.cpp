//
// Created by patch on 2020/11/27.
//


#include "VideoDecoder.h"


VideoDecoder::VideoDecoder(AVMediaType mediaType) {
    m_MediaType - mediaType;
}

VideoDecoder::~VideoDecoder() {
    if (m_RenderFrame){
        av_frame_free(&m_RenderFrame);
        m_RenderFrame = nullptr;
    }
}


bool VideoDecoder::initAVDecoder() {

    // 渲染帧
    m_RenderFrame = av_frame_alloc();

    if (m_RenderFrame == nullptr) {
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


void VideoDecoder::parseFrame(AVFrame *frame) {

    switch(m_RenderParams->renderFormat){
        case AV_PIX_FMT_RGBA:{
            sws_scale(m_SwsContext, frame->data, frame->linesize,
                      0, frame->height, m_RenderFrame->data, m_RenderFrame->linesize);

            auto *renderData = new RenderData ();
            renderData->data[0] = m_RenderFrame->data[0];

            renderData->pts = frame->pts;
            renderData->key_frame = frame->key_frame;

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


void VideoDecoder::setVideoRender(VideoRender *videoRender) {
    m_VideoRender = videoRender;
}









