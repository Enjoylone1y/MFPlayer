//
// Created by QTZ on 2021/5/11.
//

#ifndef MFPLAYER_ANATIVEWINDOWRENDER_H
#define MFPLAYER_ANATIVEWINDOWRENDER_H

#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>

#include "Base.h"
#include <queue>
#include <pthread.h>
#include "VideoRender.h"

using namespace std;


class ANativeWindowRender: public VideoRender {

public:
    ANativeWindowRender();
    ~ANativeWindowRender();

    bool initRender( JNIEnv *env, jobject surface,int videoWidth, int videoHeight);
    VideoRenderParams * getRenderParams();

    void renderVideoFrame(RenderData* renderData);

private:

    ANativeWindow *m_NativeWindow;
    ANativeWindow_Buffer m_WindowBuffer;

    VideoRenderParams *m_RenderParams;
};


#endif //MFPLAYER_ANATIVEWINDOWRENDER_H
