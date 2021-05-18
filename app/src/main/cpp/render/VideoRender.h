//
// Created by QTZ on 2021/5/18.
//

#ifndef MFPLAYER_VIDEORENDER_H
#define MFPLAYER_VIDEORENDER_H

#include "Base.h"

class VideoRender {

public:
    virtual void renderVideoFrame(RenderData* renderData) = 0;
};

#endif //MFPLAYER_VIDEORENDER_H
