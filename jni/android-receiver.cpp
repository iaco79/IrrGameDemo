
#include <jni.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>
#include <stdint.h>
#include <Box2D/Box2D.h>
#include <importgl.h>
#include <irrlicht.h>
#include <CIrrDeviceStub.h>
#include <os.h>


#include "Base2dNode.h"
#include "B2dNode.h"
#include "GameObject.h"
#include "Player.h"
#include "Box2dTest.h"

using namespace irr;
using namespace os;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;



#include <android-receiver.h>
#include <android/log.h>


extern int gAppAlive;


	void AndroidEventReceiver::CalculateWorldXY(bool reset, bool touch)
	 {
		
		 if(Context.smgr)
		{

			if(touch)
				__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "Mouse %d ScreenXY  %d,  %d",reset?1:0, MouseState.Position.X, MouseState.Position.Y);


			core::line3df ray = 
					Context.smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(MouseState.Position, Context.smgr->getActiveCamera());

			core::plane3df plane(core::vector3df(0, 0, 0), core::vector3df(0, 0, -1));

			core::vector3df mouseWorldPosition;

			if(plane.getIntersectionWithLine(ray.start, ray.getVector(), mouseWorldPosition))
			{

				if(touch)
				{
					
					__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "MouseWorldXY %.2f,  %.2f", mouseWorldPosition.X, mouseWorldPosition.Y);
				}


				if(reset)
				{
					
					if(touch )
					{
						b2Vec2 dif = b2Vec2(mouseWorldPosition.X,mouseWorldPosition.Y) - mBoxWorldXY;						
						mDifXY = dif;						
						
					}
				}
			
				if(touch)
				{
					mBoxWorldXY.x = mouseWorldPosition.X - mDifXY.x;
					mBoxWorldXY.y = mouseWorldPosition.Y - mDifXY.y;

					__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "WorldXY  %.2f,  %.2f", mBoxWorldXY.x, mBoxWorldXY.y);
						

				}
			}
		}

	 };



bool AndroidEventReceiver::OnEvent(const SEvent& event)
{
    //__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "Lolo");
    
    if (event.EventType == EET_GUI_EVENT)
    {
        s32 id = event.GUIEvent.Caller->getID();
        IGUIEnvironment* env = Context.device->getGUIEnvironment();

        //__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "Lolo 2: %d %d %d %d", event.GUIEvent.EventType, EGET_BUTTON_CLICKED, id , GUI_ID_QUIT_BUTTON);
        switch(event.GUIEvent.EventType)
        {
            case EGET_BUTTON_CLICKED:
                //__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "Lolo 3");
                switch(id)
                {
                    case GUI_ID_QUIT_BUTTON:
                        //__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "Lolo 4");
                        gAppAlive = 0;
                        return true;

                    default:
                        break;
                }
        }
    }
	else
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
    {
	
        switch(event.MouseInput.Event)
        {
			case EMIE_LMOUSE_PRESSED_DOWN:
			{

				__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "left click procesed thread id %d",  gettid() );

				MouseState.Position.X = event.MouseInput.X;
				MouseState.Position.Y = event.MouseInput.Y;
				CalculateWorldXY(true,false);
			 	
				MouseState.Position.X = event.MouseInput.X*2;
				MouseState.Position.Y = event.MouseInput.Y*2;
				CalculateWorldXY(true,true);


				{
					if(Context.test)
						Context.test->MouseDown(mBoxWorldXY);
				}
				MouseState.LeftButtonDown = true;

		
				break;
			}
			case EMIE_LMOUSE_LEFT_UP:
			{
				MouseState.LeftButtonDown = false;

				if(Context.test)
					Context.test->MouseUp(mBoxWorldXY);


				break;
			}
			case EMIE_MOUSE_MOVED:
			{
                
				//transform xy to world xy
				MouseState.Position.X = event.MouseInput.X*2;
				MouseState.Position.Y = event.MouseInput.Y*2;
				CalculateWorldXY(false,MouseState.LeftButtonDown);

				if(Context.test)
					Context.test->MouseMove(mBoxWorldXY);

				

				break;
			}
		}
	}
	else
	if (event.EventType == irr::EET_KEY_INPUT_EVENT)
	{
        if(event.KeyInput.PressedDown)
		{
			if(Context.test)
				Context.test->Keyboard(event.KeyInput.Key);			
		}
		else
		{
			if(Context.test)
				Context.test->KeyboardUp(event.KeyInput.Key);
		}
	}
	
    return false;
}
