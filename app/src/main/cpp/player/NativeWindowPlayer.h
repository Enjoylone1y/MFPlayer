//
// Created by patch on 2020/11/13.
//

#ifndef MFPLAYER_NATIVEWINDOWPLAYER_H
#define MFPLAYER_NATIVEWINDOWPLAYER_H

#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/codec.h>
#include <libavutil/error.h>
}


class NativeWindowPlayer {

private:
    const char *m_filePath;

    ANativeWindow *m_NativeWindow;

    AVFormatContext *m_FormatContext;
    AVCodecContext *m_CodecContext;
    AVCodec *m_Codec;
    AVPacket *m_Packet;
    AVStream *m_Stream;

    int m_StreamIndex;

    int m_VideoWidth;
    int m_VideoHeight;
    AVPixelFormat m_PixFormat;

public:
    bool init(const char *path,JNIEnv *env,jobject surface);
    void play();
    void destroy();
};


#endif //MFPLAYER_NATIVEWINDOWPLAYER_H
