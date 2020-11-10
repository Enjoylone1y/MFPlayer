#include <jni.h>
#include <string>

extern "C" {
#include <android/log.h>
#include <libavformat/avformat.h>
}


#define LOG_TAG "MFPlayer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL
Java_com_ezreal_mfplayer_MainActivity_getMediaFileInfo(
        JNIEnv *env,
        jobject /* this */,
        jstring file_path) {

    int ret = 0;
    AVFormatContext *fmt_ctx = NULL;

    const char *path = env->GetStringUTFChars(file_path, NULL);
    if (!path) {
        LOGE("file path is invalid");
        return env->NewStringUTF("error");
    }

    ret = avformat_open_input(&fmt_ctx, path, NULL, NULL);
    if (ret < 0) {
        LOGE("open input failed");
        return env->NewStringUTF("error");
    }

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

#ifdef __cplusplus
}
#endif