//
// Created by patch on 2020/11/13.
//

#ifndef MFPLAYER_MFPLAYER_H
#define MFPLAYER_MFPLAYER_H

#include <string>
#include <jni.h>
#include <android/log.h>
#include <queue>

#include "Utils.h"
#include "ANativeWindowRender.h"
#include "VideoDecoder.h"
#include "AudioDecoder.h"
#include "AudioSLESRender.h"


extern "C" {
#include <libavformat/avformat.h>
}

using namespace std;

class MFPlayer {

private:

    AVFormatContext *m_FormatContext;

    queue<RenderData*> *m_VideoQueue;
    queue<RenderData*> *m_AudioQueue;

    int m_VideoSteamIndex;
    int m_AudioSteamIndex;

    VideoDecoder *videoDecoder;
    ANativeWindowRender *windowRender;

    AudioDecoder *audioDecoder;
    AudioSLESRender *audioRender;


public:
    bool init(const char *path,JNIEnv *env,jobject surface);
    void play();
    void destroy();
};


#endif //MFPLAYER_MFPLAYER_H
