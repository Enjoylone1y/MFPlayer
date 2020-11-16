package com.ezreal.mfplayer

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.SurfaceHolder
import android.view.SurfaceView
import java.io.File


class MainActivity : AppCompatActivity() {

    private val TAG = "MFPlayer"

    private lateinit var surfaceView: SurfaceView

    private var mfPlayer:MFPlayer? = null
    private var playerInitSuccess = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        surfaceView = findViewById(R.id.surface_view)

        val filePath =  getExternalFilesDir(null)?.absolutePath + File.separator + "cg.mp4"

        surfaceView.holder.addCallback(object :SurfaceHolder.Callback{

            override fun surfaceCreated(holder: SurfaceHolder?) {
                Log.i(TAG,"---- surfaceCreated ----")
                mfPlayer = MFPlayer()
                playerInitSuccess = mfPlayer?.init(filePath,surfaceView.holder.surface)!!
            }

            override fun surfaceChanged(holder: SurfaceHolder?,
                format: Int, width: Int, height: Int) {
                Log.i(TAG,"---- surfaceChanged ----")
                if (playerInitSuccess){
                    mfPlayer?.play(width, height)
                }
            }

            override fun surfaceDestroyed(holder: SurfaceHolder?) {
                Log.i(TAG,"---- surfaceDestroyed ----")
                mfPlayer?.destroy()
            }
        })
    }
}