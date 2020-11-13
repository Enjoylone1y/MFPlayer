//
// Created by patch on 2020/11/13.
//

#ifndef MFPLAYER_NATIVEWINDOWPLAYER_H
#define MFPLAYER_NATIVEWINDOWPLAYER_H

#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <libavformat/avformat.h>
#include <libavcodec/codec.h>
#include <android/log.h>

class NativeWindowPlayer {

private:
    const char *m_filePath;

    ANativeWindow *m_NativeWindow;

    AVFormatContext *m_FormatContext;
    AVCodecContext *m_CodecContext;
    AVCodec *m_Codec;
    AVPacket *m_Packet;
    AVStream *m_Stream;

public:
    bool init(const char *path,JNIEnv *env,jobject surface);
    void play();
    void destroy();
};


#endif //MFPLAYER_NATIVEWINDOWPLAYER_H
