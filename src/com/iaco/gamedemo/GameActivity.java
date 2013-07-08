/*
 *
 */
package com.iaco.gamedemo;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.media.*;
import android.view.View;
import android.widget.Button;
import android.widget.TableLayout;

import java.io.IOException;

/** */
public class GameActivity extends Activity {

    private IrrlichtGLView mGLView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
      
        nativeEnvJ2C(Environment.getExternalStorageDirectory().getAbsolutePath());
        //mGLView = new IrrlichtGLView(this);
        //setContentView(mGLView);
		
		//use a framelayout 
		setContentView(R.layout.main);        
        mGLView = (IrrlichtGLView)findViewById(R.id.RTSurfaceView);
		
        nativeOnCreate();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mGLView.onPause();
        nativeOnPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mGLView.onResume();
        nativeOnResume();
    }

    @Override    
    public void onDestroy() {
        nativeOnDestroy();
        super.onDestroy();
    }

    /** load irrlicht.so */
    static {
        System.loadLibrary("gamedemo");
    }

	
	// Audio
    public static void audioInit(int sampleRate, boolean is16Bit, boolean isStereo, int desiredFrames) {
        int channelConfig = isStereo ? AudioFormat.CHANNEL_CONFIGURATION_STEREO : AudioFormat.CHANNEL_CONFIGURATION_MONO;
        int audioFormat = is16Bit ? AudioFormat.ENCODING_PCM_16BIT : AudioFormat.ENCODING_PCM_8BIT;
        int frameSize = (isStereo ? 2 : 1) * (is16Bit ? 2 : 1);
        
        Log.v("SDL", "SDL audio: wanted " + (isStereo ? "stereo" : "mono") + " " + (is16Bit ? "16-bit" : "8-bit") + " " + ((float)sampleRate / 1000f) + "kHz, " + desiredFrames + " frames buffer");
        
        // Let the user pick a larger buffer if they really want -- but ye
        // gods they probably shouldn't, the minimums are horrifyingly high
        // latency already
        desiredFrames = Math.max(desiredFrames, (AudioTrack.getMinBufferSize(sampleRate, channelConfig, audioFormat) + frameSize - 1) / frameSize);
        
        mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRate,
                channelConfig, audioFormat, desiredFrames * frameSize, AudioTrack.MODE_STREAM);
        
        audioStartThread();
        
        Log.v("SDL", "SDL audio: got " + ((mAudioTrack.getChannelCount() >= 2) ? "stereo" : "mono") + " " + ((mAudioTrack.getAudioFormat() == AudioFormat.ENCODING_PCM_16BIT) ? "16-bit" : "8-bit") + " " + ((float)mAudioTrack.getSampleRate() / 1000f) + "kHz, " + desiredFrames + " frames buffer");
    }
    
    public static void audioStartThread() {
        mAudioThread = new Thread(new Runnable() {
            public void run() {
                mAudioTrack.play();
                nativeRunAudioThread();
            }
        });
        
        // I'd take REALTIME if I could get it!
        mAudioThread.setPriority(Thread.MAX_PRIORITY);
        mAudioThread.start();
    }
    
    public static void audioWriteShortBuffer(short[] buffer) {
        for (int i = 0; i < buffer.length; ) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {
                    // Nom nom
                }
            } else {
                Log.w("SDL", "SDL audio: error return from write(short)");
                return;
            }
        }
    }
    
    public static void audioWriteByteBuffer(byte[] buffer) {
        for (int i = 0; i < buffer.length; ) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {
                    // Nom nom
                }
            } else {
                Log.w("SDL", "SDL audio: error return from write(short)");
                return;
            }
        }
    }

    public static void audioQuit() {
        if (mAudioThread != null) {
            try {
                mAudioThread.join();
            } catch(Exception e) {
                Log.v("SDL", "Problem stopping audio thread: " + e);
            }
            mAudioThread = null;

            //Log.v("SDL", "Finished waiting for audio thread");
        }

        if (mAudioTrack != null) {
            mAudioTrack.stop();
            mAudioTrack = null;
        }
		
	
    
    }
	
	public void OnButtonClick(View view)
    {
    	
    	switch(view.getId())
    	{
    		case R.id.btnReset:
    		{	
    			
    			IrrlichtEvent event = new IrrlichtEvent();;
    	    	event.mAction = 7777001; //new mode
    	    	event.mExtra = 0;  // mode breakout game
    			nativeSendEvent(event);
    			Log.i("GameDemo", "Reset");
    			
    		}
    		break;
    		case R.id.btnFigure:
    		{
    			
    			 
    			
    			IrrlichtEvent event = new IrrlichtEvent();;
    	    	event.mAction = 7777002; //new figure
    	    	event.mExtra = 0; //figure = ball
    			nativeSendEvent(event);
    			Log.i("GameDemo", "new ball");
    		}
    		break;
    		case  R.id.btnQuit:
    		{
    			
				Log.i("GameDemo", "bye bye");
    			finish();
    			android.os.Process.killProcess( android.os.Process.myPid());
    		}
    		break;
    	}
    }
	
	// Audio
    private static Thread mAudioThread;
    private static AudioTrack mAudioTrack;

	
    public native void nativeOnCreate();
    public native void nativeOnPause();
    public native void nativeOnResume();
    public native void nativeOnDestroy();
    public native void nativeInitGL();
    public native void nativeResize(int w, int h);
    public native void nativeGetStatus(IrrlichtStatus status);
    public native void nativeSendEvent(IrrlichtEvent event);
    public native void nativeEnvJ2C(String sdcardPath);

    public static native void nativeDrawIteration();
	
	
	public static native void nativeInitSDL();
	public static native void nativeRunAudioThread();
	
    
}

