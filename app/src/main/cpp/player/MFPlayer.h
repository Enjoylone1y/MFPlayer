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

extern "C" {
#include <libavformat/avformat.h>
}

using namespace std;

class MFPlayer {

private:

    AVFormatContext *m_FormatContext;

    queue<RenderData*> *m_RenderQueue;

    int m_VideoSteamIndex;
    int m_AudioSteamIndex;

    VideoDecoder *videoDecoder;
    ANativeWindowRender *render;

public:
    bool init(const char *path,JNIEnv *env,jobject surface);
    void play();
    void destroy();
};


#endif //MFPLAYER_MFPLAYER_H
