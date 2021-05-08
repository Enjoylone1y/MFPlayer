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
    SLresult result;

    SLObjectItf engineObj = NULL;
    SLEngineItf audioEngine = NULL;

    SLObjectItf outputMixObj = NULL;
    SLEnvironmentalReverbItf envReverb = NULL;

    SLObjectItf playerObj = NULL;
    SLPlayItf audioPlayer = NULL;
    SLVolumeItf playerVolume = NULL;

    SLAndroidSimpleBufferQueueItf  audioPlayerBuffQueue;

    const size_t BUFFER_SIZE = 44100 * 2 * 2;
    FILE *fileFd;
    uint8_t *outBuffer;
    void *buffer;
    size_t readSize;


public:
    void playAssetsMine(AAsset *asset);
    void playPcm(const char * filePath);
    void handlerBufferCallback();

private:
    int initEngine();
    int playerWidthType(AudioSourceType type,AAsset *asset,const char * filePath = "");
    void release();
};


#endif //MFPLAYER_AUDIOPLAYER_H
