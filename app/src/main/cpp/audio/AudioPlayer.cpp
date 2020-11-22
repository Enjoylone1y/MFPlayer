//
// Created by patch on 2020/11/20.
//

#include "AudioPlayer.h"
#include <cassert>



void audioPlayerCallback(SLAndroidSimpleBufferQueueItf outBuffer,void *context){


}


int AudioPlayer::init() {
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
    SLDataFormat_PCM dataFormatPcm = {
            SL_DATAFORMAT_PCM, // type
            2, //channelNum
            SL_SAMPLINGRATE_44_1, //simple rate
            SL_PCMSAMPLEFORMAT_FIXED_16, // bits width
            SL_PCMSAMPLEFORMAT_FIXED_16, // content size
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, // left and right channel
            SL_BYTEORDER_LITTLEENDIAN // byte order
    };

    // create data source
    SLDataSource  dataSource = {&locBufferQueue, &dataFormatPcm};

    // set outputMix
    SLDataLocator_OutputMix dataSinkLoc = {SL_DATALOCATOR_OUTPUTMIX,outputMixObj};

    // set output
    SLDataSink dataSink = {&dataSinkLoc, nullptr};

    //  config player
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME };
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    // create player
    result = (*audioEngine)->CreateAudioPlayer(audioEngine,&playerObj,&dataSource,&dataSink,3,ids,req);
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

    // get the buffer queue interface
    result = (*playerObj)->GetInterface(playerObj, SL_IID_BUFFERQUEUE, &audioPlayerBuffQueue);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    (*audioPlayerBuffQueue)->RegisterCallback(audioPlayerBuffQueue,audioPlayerCallback, this);

    return 0;
}