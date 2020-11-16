package com.ezreal.mfplayer

import android.view.Surface

class MFPlayer {

    var playerHandle:Long = -1


    fun init(filePath: String,surface: Surface):Boolean {
        playerHandle =  NativePlayerInit(filePath, surface);
        return playerHandle > 0
    }

    fun play(width:Int,height: Int) {
        NativePlayerPlay(playerHandle, width, height)
    }

    fun destroy() {
        NativePlayerDestroy(playerHandle)
    }

    external fun getMediaFileInfo(filePath:String): String

    external fun NativePlayerInit(filePath: String, surface: Surface): Long
    external fun NativePlayerPlay(playerHandle:Long,width:Int,height:Int)
    external fun NativePlayerDestroy(playerHandle:Long)

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-player")
        }
    }
}