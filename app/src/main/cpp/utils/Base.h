//
// Created by QTZ on 2021/5/8.
//

#ifndef MFPLAYER_BASE_H
#define MFPLAYER_BASE_H


extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/codec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
}

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <pthread.h>
#include <queue>
#include "Utils.h"

enum PlayerState {
    PREPARING,
    PREPARED,
    PLAYING,
    PAUSE,
    STOP
};


enum MediaType {
    MEDIA_TYPE_VIDEO,
    MEDIA_TYPE_AUDIO
};


typedef struct VideoRenderParams {
    AVPixelFormat renderFormat;
    int width,height;
} VideoRenderParams;


typedef struct AudioRenderParams {
    uint64_t channelLayout;
    AVSampleFormat simpleFormat;
    int nbSimple;
    int simpleRate;
    int channels;
} AudioRenderParams;


typedef struct RenderData {

    MediaType type;
    int64_t pts;

    /*for video*/
    int width,height;
    int format;
    int key_frame;


    uint8_t* data[AV_NUM_DATA_POINTERS] = {nullptr};
    int linesize[AV_NUM_DATA_POINTERS] = { 0 };

    /*for audio*/
    int nbSamples;
    int audioDataSize;

    uint8_t *audioData;

} RenderData;



#endif //MFPLAYER_BASE_H
