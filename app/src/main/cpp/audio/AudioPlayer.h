//
// Created by patch on 2020/11/20.
//

#ifndef MFPLAYER_AUDIOPLAYER_H
#define MFPLAYER_AUDIOPLAYER_H

#include <jni.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class AudioPlayer {
private:
    SLObjectItf engineObj = NULL;
    SLEngineItf audioEngine = NULL;

    SLObjectItf outputMixObj = NULL;
    SLEnvironmentalReverbItf envReverb = NULL;


    SLPlayItf audioPlayer = NULL;

public:
    int init();
};


#endif //MFPLAYER_AUDIOPLAYER_H
