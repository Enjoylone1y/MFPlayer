//
// Created by QTZ on 2021/5/8.
//

#ifndef MFPLAYER_UTILS_H
#define MFPLAYER_UTILS_H

extern "C" {
#include <libavutil/error.h>
};

#include <android/log.h>
#include <string>

#define LOG_TAG "MFPlayer"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


static void logError(int ret,const char* msg){

    if (ret == -1){
        LOGE("%s\n",msg);
    } else {
        char errStr[1024];
        av_strerror(ret,errStr,1024);
        LOGE("%s: %d\t%s\n",msg,ret,errStr);
    }
}

static long long getSysCurrentTime()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    long long curTime = ((long long)(time.tv_sec))*1000+time.tv_usec/1000;
    return curTime;
}

#endif //MFPLAYER_UTILS_H
