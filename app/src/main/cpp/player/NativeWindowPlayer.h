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
#include <libavcodec/avcodec.h>
#include <libavutil/error.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}


class NativeWindowPlayer {

private:

    const char *m_filePath;

    AVFormatContext *m_FormatContext;
    AVCodecContext *m_CodecContext;
    AVCodec *m_Codec;
    SwsContext *m_SwsContext;
    AVPacket *m_Packet;

    AVStream *m_Stream;
    int m_StreamIndex;

    AVFrame *m_VideoFrame;
    int m_VideoWidth;
    int m_VideoHeight;
    AVPixelFormat m_VideoPixFmt;


    ANativeWindow *m_NativeWindow;
    ANativeWindow_Buffer m_WindowBuffer;

    AVFrame *m_RenderFrame;
    int m_RenderWidth;
    int m_RenderHeight;
    const AVPixelFormat RENDER_FORMAT = AV_PIX_FMT_RGBA;

    int decoderFrameAndPlay();

public:
    bool init(const char *path,JNIEnv *env,jobject surface);
    void play();
    void destroy();
};


#endif //MFPLAYER_NATIVEWINDOWPLAYER_H
