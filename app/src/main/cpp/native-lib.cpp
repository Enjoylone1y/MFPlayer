#include <jni.h>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "MFPlayer.h"
#include "AudioPlayer.h"


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
Java_com_ezreal_mfplayer_MFPlayer_getMediaFileInfo(
        JNIEnv *env,
        jobject /* this */,
        jstring file_path) {

    int ret = 0;
    AVFormatContext *fmt_ctx = NULL;
    AVStream *stream = NULL;
    AVCodec *codec = NULL;
    AVCodecParameters *codecParser = NULL;

    av_log_set_level(AV_LOG_INFO);

    const char *path = env->GetStringUTFChars(file_path, NULL);
    if (!path) {
        LOGE("file path is invalid");
        return env->NewStringUTF("error");
    }

    fmt_ctx = avformat_alloc_context();

    ret = avformat_open_input(&fmt_ctx, path, NULL, NULL);
    if (ret < 0) {
        LOGE("open input failed");
        return env->NewStringUTF("error");
    }

    std::string output = "codecs:\n";

    for(int i =0; i < fmt_ctx->nb_streams; i++){
        stream = fmt_ctx->streams[i];
        codecParser = stream->codecpar;
        codec = avcodec_find_decoder(codecParser->codec_id);
        if(codec){
            LOGI("find codec name = %s\t%s\n",codec->name,codec->long_name);
            output += codec->name;
            output += "\n";
        }
    }

    if(fmt_ctx){
        avformat_close_input(&fmt_ctx);
    }

    return env->NewStringUTF(output.c_str());
}


JNIEXPORT jlong JNICALL
Java_com_ezreal_mfplayer_MFPlayer_NativePlayerInit(JNIEnv *env, jobject thiz,
                                                   jstring file_path, jobject surface) {
    MFPlayer *player =  new MFPlayer();
    bool success = player->init(env->GetStringUTFChars(file_path,NULL),env,surface);
    if (success){
        return reinterpret_cast<jlong>(player);
    }
    return -1;
}


JNIEXPORT void JNICALL
Java_com_ezreal_mfplayer_MFPlayer_NativePlayerPlay(JNIEnv *env, jobject thiz,jlong player_handle) {
    if (player_handle > 0){
        MFPlayer *player = reinterpret_cast<MFPlayer*>(player_handle);
        if(player){
            player->play();
        }
    }
}


JNIEXPORT void JNICALL
Java_com_ezreal_mfplayer_MFPlayer_NativePlayerDestroy(JNIEnv *env, jobject thiz,jlong player_handle){
    if (player_handle > 0){
        MFPlayer *player = reinterpret_cast<MFPlayer*>(player_handle);
        if(player){
            player->destroy();
        }
    }
}

#ifdef __cplusplus
}
#endif


extern "C"
JNIEXPORT void JNICALL
Java_com_ezreal_mfplayer_MFPlayer_NativePlayMine(JNIEnv *env, jobject thiz,jobject assets, jstring fileName) {
    const  char *file = env->GetStringUTFChars(fileName,NULL);
    AAssetManager * assetManager = AAssetManager_fromJava(env,assets);
    AAsset * asset = AAssetManager_open(assetManager,file,AASSET_MODE_UNKNOWN);
    env->ReleaseStringUTFChars(fileName,file);
    AudioPlayer *audioPlayer = new AudioPlayer();
    audioPlayer->playAssetsMine(asset);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_ezreal_mfplayer_MFPlayer_NativePlayPcm(JNIEnv *env, jobject thiz,jstring file_path) {
    const  char *filePath = env->GetStringUTFChars(file_path,NULL);
    LOGI("---- filePath: %s ----",filePath);
    AudioPlayer *audioPlayer = new AudioPlayer();
    audioPlayer->playPcm(filePath);
}