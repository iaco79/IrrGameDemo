package com.iaco.gamedemo;
import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;



/**  IrrlichtGLView */
class IrrlichtGLView extends GLSurfaceView {
    private GameActivity mActivity;
    private IrrlichtEvent lastEvent;

	private static final int INVALID_POINTER_ID = -1;
	private int mActivePointerId = INVALID_POINTER_ID;

    public IrrlichtGLView(GameActivity activity) {
        super(activity);
        mActivity = activity;
		setEGLContextClientVersion(2);
        mRenderer = new IrrlichtRenderer(activity);
        setRenderer(mRenderer);
        lastEvent = new IrrlichtEvent();
    }
	
	//static intializer
	public IrrlichtGLView(Context context, AttributeSet attrs) {
        super(context, attrs);    
        Log.i("rendertest", "RTSurface constructor - Layout Form");
        
        mActivity = (GameActivity)context;
		setEGLContextClientVersion(2);
        mRenderer = new IrrlichtRenderer(mActivity);
        setRenderer(mRenderer);
        lastEvent = new IrrlichtEvent();
     }  
	 

    public boolean onTouchEvent(final MotionEvent ev) {
  
        
        final int action = ev.getAction();
		switch (action & MotionEvent.ACTION_MASK) 
		{
			case MotionEvent.ACTION_DOWN: 
			{
		
				// Save the ID of this pointer
				mActivePointerId = ev.getPointerId(0);
			
				
				lastEvent.mAction = MotionEvent.ACTION_DOWN;        
				lastEvent.mX = ev.getX();
				lastEvent.mY = ev.getY();
				if (mRenderer!=null) {
				
				
					final float x = lastEvent.mX;
					final float y = lastEvent.mY;
				
					mActivity.nativeSendEvent(lastEvent);
				
				}
				break;
			}
			case MotionEvent.ACTION_POINTER_DOWN: 
			{
				final int pointerIndex = (action & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
				final int pointerId = ev.getPointerId(pointerIndex);
								
				
				final float x = ev.getX(pointerIndex);
				final float y = ev.getY(pointerIndex);
		       	
				if (mRenderer!=null) 
				{
					
					
					mActivePointerId = pointerId;				
					lastEvent.mAction = MotionEvent.ACTION_DOWN;        
					lastEvent.mX = x;
					lastEvent.mY = y;
					if (mRenderer!=null) 
					{
						mActivity.nativeSendEvent(lastEvent);
					}
				
				}
				break;
				 			 
			}		        
			case MotionEvent.ACTION_MOVE: 
			{
				if(mActivePointerId != INVALID_POINTER_ID)
				{
					// Find the index of the active pointer and fetch its position
					final int pointerIndex = ev.findPointerIndex(mActivePointerId);
					final float x = ev.getX(pointerIndex);
					final float y = ev.getY(pointerIndex);
			       
					lastEvent.mAction = MotionEvent.ACTION_MOVE;
					lastEvent.mX = x;
					lastEvent.mY = y;
					if (mRenderer!=null) {
						mActivity.nativeSendEvent(lastEvent);
					}            
				}

				break;
			}
		        
			case MotionEvent.ACTION_UP: 
			{
				mActivePointerId = INVALID_POINTER_ID;				
				lastEvent.mAction = MotionEvent.ACTION_UP;
				
				if (mRenderer!=null) {
					mActivity.nativeSendEvent(lastEvent);
				}            
				
				break;
			}
		        
			case MotionEvent.ACTION_CANCEL: 
			{
				mActivePointerId = INVALID_POINTER_ID;
				lastEvent.mAction = MotionEvent.ACTION_CANCEL;
				
				break;
			}
		    
			case MotionEvent.ACTION_POINTER_UP: 
			{
				// Extract the index of the pointer that left the touch sensor
				final int pointerIndex = (action & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
				final int pointerId = ev.getPointerId(pointerIndex);
				
				if (pointerId == mActivePointerId) {
					// This was our active pointer going up. Choose a new
					// active pointer and adjust accordingly.
					//final int newPointerIndex = pointerIndex == 0 ? 1 : 0;
					mActivePointerId = INVALID_POINTER_ID; //ev.getPointerId(newPointerIndex);
					
					
				
					if (mRenderer!=null) {					
						lastEvent.mAction = MotionEvent.ACTION_UP;
						mActivity.nativeSendEvent(lastEvent);
					}            
					
					
					
				}
				break;
			}
		}
	        
        return true;
    }

    IrrlichtRenderer mRenderer;




}

class IrrlichtRenderer implements GLSurfaceView.Renderer {
    private GameActivity mActivity;
    public int mWindowWidth;
    public int mWindowHeight;
    private IrrlichtStatus mStatus;

    public IrrlichtRenderer(GameActivity activity) {
        mActivity = activity;
        mStatus = new IrrlichtStatus();
        mStatus.mQuit = false;
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
	
	    
        mActivity.nativeInitGL();
		
		gl.glDisable(GL10.GL_DITHER); 
		
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mWindowWidth = width;
        mWindowHeight = height;
        mActivity.nativeResize(width, height);
    }

    public void onDrawFrame(GL10 gl) {
        // render a frame
		if(firstTime)
		{
		Log.e("MESSAGE", "Calling Native Init SDL");
		
			mActivity.nativeInitSDL();
			firstTime = false;
		}
		
        mActivity.nativeDrawIteration();
        // get status
        mActivity.nativeGetStatus(mStatus);
        if (mStatus.mQuit) {
            Log.e("MESSAGE", "QUIT");
            mActivity.finish();
        }
    }
	
	boolean firstTime = true;

}

class IrrlichtEvent {
    public int mAction;        
	public int mExtra;
    public float mX;
    public float mY;
}

class IrrlichtStatus {
    public boolean mQuit;
}
