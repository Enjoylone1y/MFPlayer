//
// Created by QTZ on 2021/5/8.
//

#ifndef MFPLAYER_BASE_H
#define MFPLAYER_BASE_H

enum PlayerState {
    PREPARING,
    PREPARED,
    PLAYING,
    PAUSE,
    STOP
};


enum RenderType {
    VIDEO,
    AUDIO
};

typedef struct RenderData {

#define AV_NUM_DATA_POINTERS 8

    RenderType type;

    /*for video*/
    int width,height;
    int format;
    int key_frame;
    int64_t pts;

    uint8_t *data[AV_NUM_DATA_POINTERS];
    int linesize[AV_NUM_DATA_POINTERS];

    /*for video*/


} RenderData;



#endif //MFPLAYER_BASE_H
