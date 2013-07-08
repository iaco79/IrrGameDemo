#ifndef __ANDROID_RECEIVER_H_INCLUDED__
#define __ANDROID_RECEIVER_H_INCLUDED__

#include <irrlicht.h>


using namespace irr;

using namespace gui;


enum
{
        GUI_ID_QUIT_BUTTON = 101,
        GUI_ID_NEW_WINDOW_BUTTON,
        GUI_ID_FILE_OPEN_BUTTON,
        GUI_ID_TRANSPARENCY_SCROLL_BAR
};



struct SAppContext
{
    IrrlichtDevice *device;
    s32				counter;
    ISceneManager* smgr;
	Box2dTest* test;	
};


class AndroidEventReceiver : public IEventReceiver
{
private:
    SAppContext& Context;
	

public:
	b2Vec2 mBoxWorldXY;
	
	b2Vec2 mDifXY;

    AndroidEventReceiver(SAppContext & context) : Context(context) 
	{ 
		mBoxWorldXY.x=0; 
		mBoxWorldXY.y=0;

		
		mDifXY.x=0;
		mDifXY.y=0;

	}

	void CalculateWorldXY(bool reset = false, bool touch=false);

	struct SMouseState
    {
        core::position2di Position;
        bool LeftButtonDown;
        SMouseState() : LeftButtonDown(false) { }
    } MouseState;


    virtual bool OnEvent(const SEvent& event);


};

#endif // __ANDROID_RECEIVER_H_INCLUDED__
