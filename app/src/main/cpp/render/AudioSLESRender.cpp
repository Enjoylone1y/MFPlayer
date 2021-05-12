//
// Created by patch on 2020/11/27.
//

#include "AudioSLESRender.h"

AudioSLESRender::AudioSLESRender() {

}

AudioSLESRender::~AudioSLESRender() {

}

static void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    LOGI("--- AudioPlayerCallback ----");
    auto *render = static_cast<AudioSLESRender*>(context);
    if (render) render->playAudio();
}


bool AudioSLESRender::initRender(queue<RenderData *> *queue) {

    m_RenderQueue = queue;

    SLDataSource  dataSource;
    SLDataSink dataSink;

    // create engine
    result = slCreateEngine(&engineObj,0, nullptr,
            0,nullptr,nullptr);
    (void)result;

    // realize engine
    result = (*engineObj)->Realize(engineObj,SL_BOOLEAN_FALSE);
    (void)result;

    // get interface
    result = (*engineObj)->GetInterface(engineObj,SL_IID_ENGINE,&audioEngine);
    (void)result;

    // create outputMix
    result = (*audioEngine)->CreateOutputMix(audioEngine,&outputMixObj,0,nullptr,nullptr);
    (void)result;

    (*outputMixObj)->Realize(outputMixObj,SL_BOOLEAN_FALSE);
    (void)result;

    // get outputMix interface
    result = (*outputMixObj)->GetInterface(outputMixObj,SL_IID_ENVIRONMENTALREVERB,&envReverb);
    if (SL_RESULT_SUCCESS == result){
        // set mix option,current will be default
        const SLEnvironmentalReverbSettings settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;
        (*envReverb)->SetEnvironmentalReverbProperties(envReverb,&settings);
    }

    // config audio source
    SLDataLocator_AndroidBufferQueue locBufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2}; //Simple Buffer Queue

    // config audio format
    SLDataFormat_PCM dataFormat = {
            SL_DATAFORMAT_PCM, // type
            2, //channelNum
            SL_SAMPLINGRATE_44_1, //simple rate
            SL_PCMSAMPLEFORMAT_FIXED_16, // bits width
            SL_PCMSAMPLEFORMAT_FIXED_16, // content size
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, // left and right
            SL_BYTEORDER_LITTLEENDIAN // byte order
    };

    // create data source
    dataSource = {&locBufferQueue, &dataFormat};

    m_RenderParams = new AudioRenderParams ();
    m_RenderParams->simpleFormat = AV_SAMPLE_FMT_S16;
    m_RenderParams->simpleRate = 44100;
    m_RenderParams->nbSimple = 1024;
    m_RenderParams->channelLayout = 2;

    // set outputMix
    SLDataLocator_OutputMix dataSinkLoc = {SL_DATALOCATOR_OUTPUTMIX,outputMixObj};
    // set output
    dataSink = {&dataSinkLoc, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME };
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*audioEngine)->CreateAudioPlayer(audioEngine,&playerObj,&dataSource,&dataSink,3,ids,req);
    (void)result;

    // realize
    result = (*playerObj)->Realize(playerObj,SL_BOOLEAN_FALSE);
    (void)result;

    // get interface
    result = (*playerObj)->GetInterface(playerObj,SL_IID_PLAY,&audioPlayer);
    (void)result;

    // get volume
    result = (*playerObj)->GetInterface(playerObj,SL_IID_VOLUME,&playerVolume);
    (void)result;

    // get the buffer queue interface
    result = (*playerObj)->GetInterface(playerObj, SL_IID_BUFFERQUEUE, &audioPlayerBuffQueue);
    (void)result;

    // set buffer callback
    result = (*audioPlayerBuffQueue)->RegisterCallback(audioPlayerBuffQueue,AudioPlayerCallback, this);
    (void)result;


    return true;
}

AudioRenderParams *AudioSLESRender::getRenderParams() {
    return m_RenderParams;
}


bool AudioSLESRender::start() {
    result = (*audioPlayer)->SetPlayState(audioPlayer,SL_PLAYSTATE_PLAYING);
    AudioPlayerCallback(audioPlayerBuffQueue, this);
    return true;
}

bool AudioSLESRender::pause() {
    return false;
}

bool AudioSLESRender::stop() {
    return false;
}


void AudioSLESRender::playAudio(){
    while (true){
        av_usleep(10);
        if (!m_RenderQueue->empty()) break;
    }
    auto *renderData = m_RenderQueue->front();
    if (renderData){
        result = (*audioPlayerBuffQueue)->Enqueue(audioPlayerBuffQueue, renderData->audioData,
                                                           (SLuint32) renderData->audioDataSize);
        if (result != SL_RESULT_SUCCESS){
            logError(-1,"PLAY ERROR");
        }
        delete renderData;
        m_RenderQueue->pop();
    }
}




