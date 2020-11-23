//
// Created by patch on 2020/11/20.
//

#include "AudioPlayer.h"
#include <cassert>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <android/log.h>

#define LOG_TAG "AudioPlayer"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


void AudioPlayer::playAssetsMine(AAsset *asset) {
    int ret = -1;

    ret = this->initEngine();
    assert(ret == 0);

    ret = this->createPlayerWidthType(MINE,asset);
    assert(ret == 0);

    this->startPlay();
}


void AudioPlayer::playPcm(AAsset *asset) {
    int ret = -1;

    ret = this->initEngine();
    assert(ret == 0);

    ret = this->createPlayerWidthType(PCM,asset);
    assert(ret == 0);

    this->startPlay();
}


void AudioPlayer::audioBufferCallback(SLAndroidSimpleBufferQueueItf outBuffer,void *context){
    LOGI("---- audioPlayerCallback ----");
    auto player = static_cast<AudioPlayer*>(context);
    if (player){
        player->handlerBufferCallback();
    }
}


void audioPlayerCallback(SLPlayItf pItf_, void *pVoid, SLuint32 status){
    LOGI("---- audioPlayerCallback %d ---- ", status);

}


int AudioPlayer::initEngine() {
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObj,0,NULL,0,NULL,NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize engine
    result = (*engineObj)->Realize(engineObj,SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get interface
    result = (*engineObj)->GetInterface(engineObj,SL_IID_ENGINE,&audioEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // create outputMix
    result = (*audioEngine)->CreateOutputMix(audioEngine,&outputMixObj,0,NULL,NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    (*outputMixObj)->Realize(outputMixObj,SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get outputMix interface
    result = (*outputMixObj)->GetInterface(outputMixObj,SL_IID_ENVIRONMENTALREVERB,&envReverb);
    if (SL_RESULT_SUCCESS == result){
        // set mix option,current will be default
        const SLEnvironmentalReverbSettings settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;
        (*envReverb)->SetEnvironmentalReverbProperties(envReverb,&settings);
    }

    return 0;
}



int AudioPlayer::createPlayerWidthType(AudioSourceType type, AAsset *asset) {

    SLresult result;

    off_t start, length;

    SLDataSource  dataSource;
    SLDataSink dataSink;

    m_Asset = asset;

    if (type == MINE){
        int fileFd = AAsset_openFileDescriptor(asset,&start,&length);
        assert(fileFd >= 0);
        SLDataLocator_AndroidFD locAndroidFd = {SL_DATALOCATOR_ANDROIDFD,fileFd,start,length};
        SLDataFormat_MIME dataFormat = {SL_DATAFORMAT_MIME,NULL,SL_RESULT_CONTENT_UNSUPPORTED};
        dataSource = {&locAndroidFd,&dataFormat};
    } else if (type == PCM){
        // config audio source
        SLDataLocator_AndroidBufferQueue locBufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,1}; //Simple Buffer Queue

        // config audio format
        SLDataFormat_PCM dataFormat = {
            SL_DATAFORMAT_PCM, // type
            2, //channelNum
            SL_SAMPLINGRATE_44_1, //simple rate
            SL_PCMSAMPLEFORMAT_FIXED_16, // bits width
            SL_PCMSAMPLEFORMAT_FIXED_16, // content size
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, // left and right channel
            SL_BYTEORDER_LITTLEENDIAN // byte order
        };

        // create data source
        dataSource = {&locBufferQueue, &dataFormat};
    }

    // set outputMix
    SLDataLocator_OutputMix dataSinkLoc = {SL_DATALOCATOR_OUTPUTMIX,outputMixObj};
    // set output
    dataSink = {&dataSinkLoc, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_EFFECTSEND, SL_IID_VOLUME };
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    // create player
    result = (*audioEngine)->CreateAudioPlayer(audioEngine,&playerObj,&dataSource,&dataSink,2,ids,req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize
    result = (*playerObj)->Realize(playerObj,SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get interface
    result = (*playerObj)->GetInterface(playerObj,SL_IID_PLAY,&audioPlayer);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get volume
    result = (*playerObj)->GetInterface(playerObj,SL_IID_VOLUME,&playerVolume);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    if (type == PCM){
        // get the buffer queue interface
        result = (*playerObj)->GetInterface(playerObj, SL_IID_BUFFERQUEUE, &audioPlayerBuffQueue);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;

        // set buffer callback
        (*audioPlayerBuffQueue)->RegisterCallback(audioPlayerBuffQueue,audioBufferCallback, this);
    }

    // set player callback
    result = (*audioPlayer)->RegisterCallback(audioPlayer,audioPlayerCallback, this);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;


    return 0;
}


void AudioPlayer::startPlay() {
    // set player state playing
    SLresult result = (*audioPlayer)->SetPlayState(audioPlayer,SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}



void AudioPlayer::stopPlay() {

}


void AudioPlayer::getPcmBuffer() {
    if (m_Asset){
        
    }
}

void AudioPlayer::handlerBufferCallback() {

}
