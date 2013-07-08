#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include <irrlicht.h>
#include <CIrrDeviceStub.h>

#include <Box2D/Box2D.h>
#include "Base2dNode.h"
#include "B2dNode.h"
#include "GameObject.h"
#include "Player.h"
#include "Audio/AudioManager.h" 
#include "Box2dTest.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define ACTION_DOWN 0x00
#define ACTION_UP 0x01
#define ACTION_MOVE 0x02
#define ACTION_VIRTUAL_BUTTON 7777000
#define ACTION_VIRTUAL_BUTTON_RESET 7777001
#define ACTION_VIRTUAL_BUTTON_FIGURE 7777002
#define ACTION_VIRTUAL_BUTTON_ADD 7777003


extern "C" void SDL_Android_Init(JNIEnv* env, jclass cls);

extern "C" void SDL_Android_Init_AudioThread(JNIEnv* mEnv, jclass cls);

int importGLInit();
void importGLDeinit();
void nativeDrawIteration();

// global variables
int  gWindowWidth  = 320;
int  gWindowHeight = 480;
int  gAppAlive   = 1;
int  lastTouchX;
int  lastTouchY;





stringc gSdCardPath = "/sdcard/";


IrrlichtDevice* device = NULL;
IVideoDriver* driver = NULL;


/* For JNI: C++ compiler need this */
extern "C" {

/** Activity onCreate */
void Java_com_iaco_gamedemo_GameActivity_nativeOnCreate( JNIEnv*  env )
{
    
}

/** Activity onPause */
void Java_com_iaco_gamedemo_GameActivity_nativeOnPause( JNIEnv*  env )
{

}

/** Activity onResume */
void Java_com_iaco_gamedemo_GameActivity_nativeOnResume( JNIEnv*  env )
{
    
}

//IACO add to init SDL

void Java_com_iaco_gamedemo_GameActivity_nativeRunAudioThread( JNIEnv*  env, jclass cls, jobject obj )
{
	SDL_Android_Init_AudioThread(env, cls);

}
void Java_com_iaco_gamedemo_GameActivity_nativeInitSDL( JNIEnv*  env, jclass cls, jobject obj )
{
    /* This interface could expand with ABI negotiation, calbacks, etc. */
    SDL_Android_Init(env, cls);
}

/** Activity onDestroy */
void Java_com_iaco_gamedemo_GameActivity_nativeOnDestroy( JNIEnv*  env )
{
    importGLDeinit();
}

void Java_com_iaco_gamedemo_GameActivity_nativeInitGL( JNIEnv*  env )
{
    importGLInit();

	lastTouchX=0;
	lastTouchY=0;

    device = createDevice( video::EDT_OGLES2, dimension2d<u32>(gWindowWidth, gWindowHeight), 16, false, false, false, NULL);
	
    driver = device->getVideoDriver();
    
	__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "createDevice r=%d w=%d h=%d", device, gWindowWidth, gWindowHeight);
	__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "getVideoDriver r=%d", driver);

    if (!device)
        __android_log_print(ANDROID_LOG_INFO, "Irrlicht", "No device");
    if (!driver)
        __android_log_print(ANDROID_LOG_INFO, "Irrlicht", "No driver");
}

void Java_com_iaco_gamedemo_GameActivity_nativeResize( JNIEnv*  env, jobject  thiz, jint w, jint h )
{
    __android_log_print(ANDROID_LOG_INFO, "Irrlicht", "resize w=%d h=%d", w, h);
    gWindowWidth  = w;
    gWindowHeight = h;
    irr::core::dimension2d<unsigned int> size;
    size.Width = w;
    size.Height = h; 
    device->getVideoDriver()->OnResize(size);
}

void Java_com_iaco_gamedemo_GameActivity_nativeSendEvent( JNIEnv*  env, jobject defaultObj, jobject event) 
{
    jclass classEvent = env->GetObjectClass(event);
    jfieldID fieldAction = env->GetFieldID(classEvent, "mAction", "I");
    jfieldID fieldX = env->GetFieldID(classEvent, "mX", "F");
    jfieldID fieldY = env->GetFieldID(classEvent, "mY", "F");
	jfieldID fieldExtra = env->GetFieldID(classEvent, "mExtra", "I");

	
    int action = env->GetIntField(event, fieldAction);
	int extra = env->GetIntField(event, fieldExtra);
    // convert Android coord to OpenGL coords
    float x = env->GetFloatField(event, fieldX);
    float y = env->GetFloatField(event, fieldY);
    SEvent irrevent;
    irrevent.MouseInput.ButtonStates = 0xffffffff;
    irrevent.EventType = irr::EET_MOUSE_INPUT_EVENT;
    irrevent.MouseInput.Event = irr::EMIE_COUNT;
	irrevent.MouseInput.X = x;
	irrevent.MouseInput.Y = y;

	if(action == ACTION_VIRTUAL_BUTTON_RESET)
	{
		irrevent.EventType = irr::EET_KEY_INPUT_EVENT;
		irrevent.KeyInput.PressedDown = false;

		switch (extra)
		{
			case 0: //reset breakout game
				irrevent.KeyInput.Key = irr::KEY_KEY_W;
				break;			
		}
		
		irrevent.KeyInput.PressedDown = false;
	    ((CIrrDeviceStub*) device)->enqueueEventsFromUser(irrevent);			
	}
	else if(action == ACTION_VIRTUAL_BUTTON_FIGURE)
	{
		irrevent.EventType = irr::EET_KEY_INPUT_EVENT;
		irrevent.KeyInput.PressedDown = true;

		switch (extra)
		{
			case 0: //new ball
				irrevent.KeyInput.Key = irr::KEY_KEY_1;
				break;			
		}
		
	    ((CIrrDeviceStub*) device)->enqueueEventsFromUser(irrevent);			
	}
	else  if (action==ACTION_DOWN)
	{
		lastTouchX=x;
		lastTouchY=y;

        irrevent.MouseInput.ButtonStates = 0;
	    irrevent.MouseInput.ButtonStates = irr::EMBSM_LEFT;
        irrevent.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN; 
		irrevent.MouseInput.ButtonStates ^= irr::EMBSM_LEFT;   

		__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "left click received thread id %d",  gettid() );


		((CIrrDeviceStub*) device)->enqueueEventsFromUser(irrevent);
				
    } 
	else if (action==ACTION_UP)
	{
		lastTouchX=x;
		lastTouchY=y;

        irrevent.MouseInput.ButtonStates = 0;
        irrevent.MouseInput.ButtonStates = irr::EMBSM_LEFT;
        irrevent.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
		irrevent.MouseInput.ButtonStates ^= irr::EMBSM_LEFT;  
        //device->postEventFromUser(irrevent);
		((CIrrDeviceStub*) device)->enqueueEventsFromUser(irrevent);
     //   __android_log_print(ANDROID_LOG_INFO, "Irrlicht", "left click release %d %f %f", action, x, y);
    } 	
	else if (action==ACTION_MOVE)
	{ //move

		if(x!= lastTouchX && y!=lastTouchY)
		{
			irrevent.MouseInput.ButtonStates = 0;
			irrevent.MouseInput.ButtonStates = irr::EMBSM_LEFT;
			irrevent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
			irrevent.MouseInput.ButtonStates ^= irr::EMBSM_LEFT;  
			//device->postEventFromUser(irrevent);
			((CIrrDeviceStub*) device)->enqueueEventsFromUser(irrevent);
			lastTouchX=x;
			lastTouchY=y;
		}
		

      //  __android_log_print(ANDROID_LOG_INFO, "Irrlicht", "mouse move event %d %f %f", action, x, y);
	}
	else 
	{
        //__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "not handled %d", action);   
    }    
}

void Java_com_iaco_gamedemo_GameActivity_nativeGetStatus(JNIEnv*  env, jobject defaultObj, jobject status) 
{
    if (gAppAlive==0) {
        jclass classStatus = env->GetObjectClass(status);
        jfieldID fieldStatus = env->GetFieldID(classStatus, "mQuit", "Z");
        env->SetBooleanField(status, fieldStatus, true);
    }    
}


void Java_com_iaco_gamedemo_GameActivity_nativeEnvJ2C(JNIEnv*  env, jobject defaultObj, jstring jsdcardPath) 
{
    char ligne[1024+1];
    const char *msg = env->GetStringUTFChars(jsdcardPath,0);
    gSdCardPath = msg;
    __android_log_print(ANDROID_LOG_INFO, "Irrlicht", "not handled %s", gSdCardPath.c_str());
    env->ReleaseStringUTFChars(jsdcardPath,msg);
}

void Java_com_iaco_gamedemo_GameActivity_nativeDrawIteration( JNIEnv*  env ) 
{
    nativeDrawIteration();   
}

}
