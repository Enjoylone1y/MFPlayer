//
// Created by patch on 2020/11/13.
//

#include "NativeWindowPlayer.h"
#include <libavutil/error.h>
#include <android/native_window_jni.h>

#define LOG_TAG "MFPlayer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

void logError(int ret){
    char errStr[1024];
    av_strerror(ret,errStr,1024);
    LOGE("open input failed: %d\t%s\n",ret,errStr);
}

bool NativeWindowPlayer::init(const char *path,JNIEnv *env,jobject surface) {

    int ret = 0;


    m_FormatContext = avformat_alloc_context();
    ret = avformat_open_input(&m_FormatContext,path,NULL,NULL);
    if (ret < 0){
        logError(ret);
        return false;
    }

    if ((ret = avformat_find_stream_info(m_FormatContext,NULL))< 0){
        logError(ret);
        return false;
    }

    //初始化NativeWindow
    m_NativeWindow = ANativeWindow_fromSurface(env,surface);

    return true;
}

void NativeWindowPlayer::play() {

}

void NativeWindowPlayer::destroy() {

}



