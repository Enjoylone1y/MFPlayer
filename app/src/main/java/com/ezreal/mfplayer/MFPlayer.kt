package com.ezreal.mfplayer

import android.content.res.AssetManager
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


    fun playMine(assetManager: AssetManager,filePath: String) {
        NativePlayMine(assetManager, filePath)
    }

    fun playPcmFile(filePath: String){
        NativePlayPcm(filePath)
    }

    external fun getMediaFileInfo(filePath:String): String

    external fun NativePlayerInit(filePath: String, surface: Surface): Long
    external fun NativePlayerPlay(playerHandle:Long,width:Int,height:Int)
    external fun NativePlayerDestroy(playerHandle:Long)

    external fun NativePlayMine(assetManager: AssetManager, filePath: String)
    external fun NativePlayPcm(filePath: String)

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-player")
        }
    }
}