//
// Created by patch on 2020/11/20.
//

#ifndef MFPLAYER_AUDIOPLAYER_H
#define MFPLAYER_AUDIOPLAYER_H

#include <jni.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/asset_manager.h>
#include <stdio.h>

enum AudioSourceType {
        MINE,
        PCM
};


class AudioPlayer {

private:
    SLObjectItf engineObj = NULL;
    SLEngineItf audioEngine = NULL;

    SLObjectItf outputMixObj = NULL;
    SLEnvironmentalReverbItf envReverb = NULL;

    SLObjectItf playerObj = NULL;
    SLPlayItf audioPlayer = NULL;
    SLVolumeItf playerVolume = NULL;

    SLAndroidSimpleBufferQueueItf  audioPlayerBuffQueue;

    AAsset *m_Asset;
    void *buffer;
    int bufferSize;

public:
    void playAssetsMine(AAsset *asset);
    void playPcm(AAsset *pAsset);
    void getPcmBuffer();

private:
    int initEngine();
    int createPlayerWidthType(AudioSourceType type,AAsset *asset);
    void startPlay();
    void stopPlay();
    void handlerBufferCallback();
    static void audioBufferCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context);
};


#endif //MFPLAYER_AUDIOPLAYER_H
