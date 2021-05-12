//
// Created by QTZ on 2021/5/11.
//

#include "ANativeWindowRender.h"
#include "Utils.h"


ANativeWindowRender::ANativeWindowRender() {

}

ANativeWindowRender::~ANativeWindowRender() {

}


bool ANativeWindowRender::initRender( JNIEnv *env, jobject surface,int videoWidth, int videoHeight,queue<RenderData*> *queue) {

    m_RenderQueue = queue;

    //初始化NativeWindow
    m_NativeWindow = ANativeWindow_fromSurface(env,surface);
    if (!m_NativeWindow){
        logError(-1,"Failed allow native window");
        return false;
    }

    int renderWidth;
    int renderHeight;

    // 获取窗口大小，计算最终渲染大小
    int windowWidth = ANativeWindow_getWidth(m_NativeWindow);
    int windowHeight = ANativeWindow_getHeight(m_NativeWindow);

    if (windowWidth / windowHeight < videoWidth / videoHeight){
        renderWidth = windowWidth;
        renderHeight = windowWidth * videoHeight / videoWidth;
    } else {
        renderWidth = windowHeight * videoWidth / videoHeight;
        renderHeight = windowHeight;
    }

    m_RenderParams = new VideoRenderParams ();
    m_RenderParams->renderFormat = AV_PIX_FMT_RGBA;
    m_RenderParams->width = renderWidth;
    m_RenderParams->height = renderHeight;

    int result =  ANativeWindow_setBuffersGeometry(m_NativeWindow,renderWidth,renderHeight,
                                                   WINDOW_FORMAT_RGBA_8888);
    if (result < 0){
        logError(-1,"Failed to set window geometry");
        return false;
    }

    return true;
}


VideoRenderParams* ANativeWindowRender::getRenderParams(){
    return m_RenderParams;
}


bool ANativeWindowRender::start() {
    renderLoop();
    return true;
}

bool ANativeWindowRender::pause() {
    return false;
}

bool ANativeWindowRender::stop() {
    return false;
}

void ANativeWindowRender::renderLoop() {
    for (;;) {
        if (m_State == STOP){
            break;
        }
        if (m_RenderQueue->empty()){
            continue;
        }
        RenderData *renderData = m_RenderQueue->front();
        if (renderData){

            LOGI("receive renderData width:%d,height:%d  lineSize:[%d,%d,%d]",
                 renderData->width, renderData->height, renderData->linesize[0],
                 renderData->linesize[1], renderData->linesize[2]);

            // 锁定缓冲区，拷贝数据
            int result = ANativeWindow_lock(m_NativeWindow,&m_WindowBuffer, nullptr);
            if (result < 0){
                logError(-1,"Failed to lock window");
            }

            //获取window buffer
            auto windowBuffer = (uint8_t *)m_WindowBuffer.bits;

            // 获取输入数据的步长
            int renderLineSize = renderData->linesize[0];
            // window 缓冲区步长
            int bufferLineSize = m_WindowBuffer.stride * 4;

            // 逐行拷贝数据
            for (int k = 0; k < renderData->height; ++k) {
                memcpy(windowBuffer + k * bufferLineSize, renderData->data[0] + k * renderLineSize, renderLineSize);
            }

            // 释放缓冲区，刷新页面
            ANativeWindow_unlockAndPost(m_NativeWindow);

            // 渲染完毕，删除对象
            m_RenderQueue->pop();
            delete renderData;
        }
    }
}
