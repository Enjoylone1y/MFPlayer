//
// Created by patch on 2020/11/27.
//

#include "AudioSLESRender.h"

AudioSLESRender::AudioSLESRender() {

}

AudioSLESRender::~AudioSLESRender() {

}

static void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    auto *render = static_cast<AudioSLESRender*>(context);
    if (render) render->playAudio();
}


bool AudioSLESRender::initRender() {

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

    const SLInterfaceID mixIds[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mixReq[1] = {SL_BOOLEAN_FALSE};
    // create outputMix
    result = (*audioEngine)->CreateOutputMix(audioEngine,&outputMixObj,1,mixIds,mixReq);
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

    m_RenderParams = new AudioRenderParams ();
    m_RenderParams->simpleFormat = AV_SAMPLE_FMT_S16;
    m_RenderParams->simpleRate = 44100;
    m_RenderParams->nbSimple = 1024;
    m_RenderParams->channelLayout = AV_CH_LAYOUT_STEREO;
    m_RenderParams->channels = 2;

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

    // set outputMix
    SLDataLocator_OutputMix dataSinkLoc = {SL_DATALOCATOR_OUTPUTMIX,outputMixObj};
    // set output
    dataSink = {&dataSinkLoc, nullptr};

    const SLInterfaceID playerIds[2] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
    const SLboolean playerReq[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*audioEngine)->CreateAudioPlayer(audioEngine,&playerObj,&dataSource,&dataSink,2,playerIds,playerReq);
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
    result = (*playerObj)->GetInterface(playerObj, SL_IID_BUFFERQUEUE, &audioBuffQueue);
    (void)result;

    // set buffer callback
    result = (*audioBuffQueue)->RegisterCallback(audioBuffQueue, AudioPlayerCallback, this);
    (void)result;


    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);
    m_RenderQueue = new queue<RenderData*>();

    return true;
}

AudioRenderParams *AudioSLESRender::getRenderParams() {
    return m_RenderParams;
}


void AudioSLESRender::renderAudioFrame(RenderData* renderData){
    while (m_RenderQueue->size() > 10){
        av_usleep(10);
    }
    pthread_mutex_lock(&mutex);
    m_RenderQueue->push(renderData);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}


bool AudioSLESRender::start() {
    while (m_RenderQueue->size() < 10 ){
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        pthread_mutex_unlock(&mutex);
    }
    result = (*audioPlayer)->SetPlayState(audioPlayer,SL_PLAYSTATE_PLAYING);
    AudioPlayerCallback(audioBuffQueue, this);
    return true;
}


bool AudioSLESRender::pause() {
    return false;
}


bool AudioSLESRender::stop() {
    return false;
}


void AudioSLESRender::playAudio(){
    while (m_RenderQueue->size() < 10 ){
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_lock(&mutex);
    auto *renderData = m_RenderQueue->front();
    if (renderData){

        LOGI("audioFramePts:%ld",renderData->pts);

        result = (*audioBuffQueue)->Enqueue(audioBuffQueue, renderData->audioData,
                                            (SLuint32) renderData->audioDataSize);
        if (result != SL_RESULT_SUCCESS){
            logError(-1,"PLAY ERROR");
        }
        m_RenderQueue->pop();
        delete [] renderData->audioData;
        renderData->audioData = nullptr;
        delete renderData;
    }
    pthread_mutex_unlock(&mutex);
}




