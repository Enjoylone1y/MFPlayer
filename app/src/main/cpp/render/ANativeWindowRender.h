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

using namespace std;


class ANativeWindowRender {
public:
    ANativeWindowRender();
    ~ANativeWindowRender();

    bool initRender( JNIEnv *env, jobject surface,int videoWidth, int videoHeight, queue<RenderData*> *queue);
    VideoRenderParams * getRenderParams();

    bool start();
    bool pause();
    bool stop();

    static void *threadFunc(void* renderInst);

private:

    pthread_t m_Thread;

    PlayerState m_State;
    queue<RenderData*> *m_RenderQueue;

    ANativeWindow *m_NativeWindow;
    ANativeWindow_Buffer m_WindowBuffer;

    VideoRenderParams *m_RenderParams;

    void renderLoop();
};


#endif //MFPLAYER_ANATIVEWINDOWRENDER_H
