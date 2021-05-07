package com.ezreal.mfplayer

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.SurfaceHolder
import android.widget.Button
import java.io.File


class MainActivity : AppCompatActivity() {

    private val TAG = "MFPlayer"

    private lateinit var surfaceView: MySurfaceView
    private lateinit var btnVideo: Button
    private lateinit var btnMine:Button
    private lateinit var btnPcm:Button

    private lateinit var mfPlayer:MFPlayer

    private var playerInitSuccess = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        surfaceView = findViewById(R.id.surface_view)
        btnVideo = findViewById(R.id.btn_video)
        btnMine = findViewById(R.id.btn_play_mine)
        btnPcm = findViewById(R.id.btn_play_pcm)

        mfPlayer = MFPlayer()

        Log.i(TAG,"---- btnVideo onClick ----")
        surfaceView.holder.addCallback(object :SurfaceHolder.Callback{

            override fun surfaceCreated(holder: SurfaceHolder?) {
                Log.i(TAG,"---- surfaceCreated ----")

                val filePath =  filesDir?.absolutePath + File.separator + "cg.mp4"
                playerInitSuccess = mfPlayer.init(filePath,surfaceView.holder.surface)
            }

            override fun surfaceChanged(holder: SurfaceHolder?,
                                        format: Int, width: Int, height: Int) {
                Log.i(TAG,"---- surfaceChanged ----")

            }

            override fun surfaceDestroyed(holder: SurfaceHolder?) {
                Log.i(TAG,"---- surfaceDestroyed ----")
                    mfPlayer.destroy()
            }
        })


        btnVideo.setOnClickListener {
            if (playerInitSuccess){
                mfPlayer.play()
            }
        }

        btnMine.setOnClickListener {
            mfPlayer.playMine(assets,"cg.pcm")
        }


        btnPcm.setOnClickListener {
            val filePath =  getExternalFilesDir(null)?.absolutePath + File.separator + "cg.pcm"
            mfPlayer.playPcmFile(filePath)
        }
    }
}