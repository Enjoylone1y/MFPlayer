//
// Created by patch on 2020/11/27.
//

#include "AudioDecoder.h"

AudioDecoder::AudioDecoder() {

}

AudioDecoder::~AudioDecoder() {

}

bool AudioDecoder::initDecoder(AVFormatContext *fmt_ctx, AudioRenderParams *params,
                               queue<RenderData *> *renderQueue) {
    int ret = 0;

    m_FormatContext = fmt_ctx;
    m_RenderQueue = renderQueue;
    m_RenderParams = params;

    ret = m_StreamIndex = av_find_best_stream(m_FormatContext, AVMEDIA_TYPE_AUDIO,
                                              -1, -1, nullptr, 0);
    if (ret < 0) {
        logError(ret, "Failed to find any audio stream");
        return false;
    }

    LOGI("---- Find audio stream index:%d ----", m_StreamIndex);

    m_Stream = m_FormatContext->streams[m_StreamIndex];


    // 初始化解码器
    m_Codec = avcodec_find_decoder(m_Stream->codecpar->codec_id);
    if (!m_Codec) {
        logError(-1, "Failed to find decoder");
        return false;
    }

    LOGI("---- Find audio decoder:%s ----", m_Codec->name);

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

    m_AudioFrame = av_frame_alloc();

    /*音频专属*/

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

    m_FrameDataSize = av_samples_get_buffer_size(nullptr,m_RenderParams->channelLayout,
            m_nbSamples,m_RenderParams->simpleFormat,1);

    m_AudioOutBuffer = (uint8_t *) malloc(m_FrameDataSize);

    return true;
}


void AudioDecoder::updateState(PlayerState newState) {

}


PlayerState AudioDecoder::getState() {
    return PREPARED;
}


void AudioDecoder::start() {
    if (!m_Thread) {
        pthread_create(&m_Thread, nullptr, threadFunc, this);
    }
}

void AudioDecoder::resume() {

}

void AudioDecoder::pause() {

}

void AudioDecoder::stop() {

}

void *AudioDecoder::threadFunc(void *decoderInst) {
    auto *decoder = static_cast<AudioDecoder*>(decoderInst);
    if (decoder){
        decoder->loopDecode();
    }
    return nullptr;
}

int AudioDecoder::loopDecode() {
    int ret = 0;
    do {
        // 队列达到一定值先休眠10MS暂停解码
        if (m_RenderQueue->size() > 100) {
            av_usleep(10);
            continue;
        }
        ret = av_read_frame(m_FormatContext, m_Packet);
        if (m_Packet->stream_index == m_StreamIndex) {
            ret = avcodec_send_packet(m_CodecContext, m_Packet);
            while (ret >= 0) {
                ret = avcodec_receive_frame(m_CodecContext, m_AudioFrame);
                if (ret < 0) {
                    logError(ret, "Error on decoding");
                } else if(ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
                    
                } else {
                    LOGI("receive audioFrame format:%d,channel:%ld,nbSimple:%d,simpleRate:%d",
                         m_AudioFrame->format,m_AudioFrame->channel_layout,m_AudioFrame->nb_samples
                    ,m_AudioFrame->sample_rate);
                    parseFrame();
                }
            }
        }
        av_packet_unref(m_Packet);
    } while (m_State != STOP);

    return ret;
}


void AudioDecoder::parseFrame() {
    int ret = swr_convert(m_SwrContext,&m_AudioOutBuffer,m_FrameDataSize / 2,
            (const uint8_t**) m_AudioFrame->data,m_AudioFrame->nb_samples);
    if (ret < 0){
        logError(ret,"swr_convert error");
        return;
    }
    auto *renderData = new RenderData ();
    renderData->nbSamples = m_nbSamples;
    renderData->audioDataSize = m_FrameDataSize;
    renderData->audioData = m_AudioOutBuffer;
    m_RenderQueue->push(renderData);
}
