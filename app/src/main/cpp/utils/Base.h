//
// Created by QTZ on 2021/5/8.
//

#ifndef MFPLAYER_BASE_H
#define MFPLAYER_BASE_H



extern "C" {
#include <libavutil/pixfmt.h>
#include <libavutil/frame.h>
};

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



typedef struct RenderData {

    MediaType type;

    /*for video*/
    int width,height;
    int format;
    int key_frame;
    int64_t pts;

    uint8_t* data[AV_NUM_DATA_POINTERS];
    int linesize[AV_NUM_DATA_POINTERS];

    /*for audio*/

    RenderData(){
        for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) {
            data[i] = nullptr;
            linesize[i] = 0;
        }
    }
} RenderData;



#endif //MFPLAYER_BASE_H
