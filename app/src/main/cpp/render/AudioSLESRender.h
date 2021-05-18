//
// Created by patch on 2020/11/27.
//

#ifndef MFPLAYER_AUDIOSLESRENDER_H
#define MFPLAYER_AUDIOSLESRENDER_H

#include "Base.h"
#include "AudioRender.h"

using namespace std;

class AudioSLESRender : public AudioRender{

public:
    AudioSLESRender();
    ~AudioSLESRender();

    bool initRender();
    AudioRenderParams * getRenderParams();

    void renderAudioFrame(RenderData* renderData);

    bool start();
    bool pause();
    bool stop();

    void playAudio();

private:

    pthread_mutex_t mutex;
    pthread_cond_t cond;

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
};


#endif //MFPLAYER_AUDIOSLESRENDER_H
