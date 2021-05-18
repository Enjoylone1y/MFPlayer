//
// Created by QTZ on 2021/5/18.
//

#ifndef MFPLAYER_AUDIORENDER_H
#define MFPLAYER_AUDIORENDER_H

#include "Base.h"

class AudioRender {

public:
    virtual void renderAudioFrame(RenderData* renderData) = 0;
};


#endif //MFPLAYER_AUDIORENDER_H
