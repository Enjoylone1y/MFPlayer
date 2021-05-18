//
// Created by patch on 2020/11/27.
//

#ifndef MFPLAYER_AUDIOSLESRENDER_H
#define MFPLAYER_AUDIOSLESRENDER_H

#include "Base.h"

using namespace std;

class AudioSLESRender {

public:
    AudioSLESRender();
    ~AudioSLESRender();

    bool initRender( queue<RenderData*> *queue );
    AudioRenderParams * getRenderParams();

    bool start();
    bool pause();
    bool stop();

    void playAudio();

private:

    queue<RenderData*> *m_RenderQueue;

    AudioRenderParams *m_RenderParams;

    SLresult result;
    SLObjectItf engineObj;
    SLEngineItf audioEngine;

    SLObjectItf outputMixObj;
    SLEnvironmentalReverbItf envReverb;

    SLObjectItf playerObj = NULL;
    SLPlayItf audioPlayer = NULL;
    SLVolumeItf playerVolume = NULL;

    SLAndroidSimpleBufferQueueItf  audioBuffQueue;

    pthread_mutex_t mutex;
};


#endif //MFPLAYER_AUDIOSLESRENDER_H
