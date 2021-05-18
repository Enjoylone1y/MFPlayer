//
// Created by patch on 2020/11/27.
//

#include "AudioDecoder.h"

AudioDecoder::AudioDecoder(AVMediaType mediaType) {
    m_MediaType = mediaType;
}

AudioDecoder::~AudioDecoder() {

}

bool AudioDecoder::initAVDecoder() {
    m_SwrContext = swr_alloc();

    // 设置转换器输入输出参数
    av_opt_set_int(m_SwrContext,"in_channel_layout", m_CodecContext->channel_layout, 0);
    av_opt_set_int(m_SwrContext,"out_channel_layout", m_RenderParams->channelLayout, 0);

    av_opt_set_int(m_SwrContext, "in_sample_rate", m_CodecContext->sample_rate, 0);
    av_opt_set_int(m_SwrContext, "out_sample_rate", m_RenderParams->simpleRate, 0);

    av_opt_set_sample_fmt(m_SwrContext, "in_sample_fmt", m_CodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(m_SwrContext, "out_sample_fmt", m_RenderParams->simpleFormat,  0);

    swr_init(m_SwrContext);

    m_nbSamples = (int) av_rescale_rnd(m_RenderParams->nbSimple, m_RenderParams->simpleRate,
                                       m_CodecContext->sample_rate,AV_ROUND_UP);

    m_FrameDataSize = av_samples_get_buffer_size(nullptr,m_RenderParams->channels,
                                                 m_nbSamples,m_RenderParams->simpleFormat,1);

    m_AudioOutBuffer = (uint8_t *) malloc(m_FrameDataSize);

    return true;
}


void AudioDecoder::parseFrame(AVFrame *frame) {

    int ret = swr_convert(m_SwrContext,&m_AudioOutBuffer,m_RenderParams->nbSimple,
                          (const uint8_t**) frame->data,frame->nb_samples);
    if (ret < 0){
        logError(ret,"swr_convert error");
        return;
    }

    auto *renderData = new RenderData ();
    renderData->pts = frame->pts;
    renderData->nbSamples = m_nbSamples;
    renderData->audioDataSize = m_FrameDataSize;
    renderData->audioData = (uint8_t*) malloc(m_FrameDataSize);
    memcpy(renderData->audioData,m_AudioOutBuffer,m_FrameDataSize);

    m_AudioRender->renderAudioFrame(renderData);
}




void AudioDecoder::setAudioRender(AudioRender *audioRender) {
    m_AudioRender = audioRender;
}
