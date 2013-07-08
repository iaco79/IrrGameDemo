/*
  Copyright (C) 2013 Othon Insauste

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
 
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Othon Insauste iaco79@gmail.com
*/
#include <windows.h>
#include <new>
#include <irrlicht.h>
#include "COGLES2Driver.h"
#include "CNullDriver.h"

#include <Box2D/Box2D.h>

#include "Base2dNode.h"
#include "B2dNode.h"
#include "SpriteNode.h"
#include "FireworkNode.h"
#include "GameObject.h"
#include "BatchedSpriteNode.h"
#include "Player.h"
#include "Box2dTest.h"
#include "Audio/AudioManager.h"
#include <Rocket/Core.h>
#include "RocketInterface/CIrrRocketGUI.h"

// #include <vld.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
#include "Materials/FireworkMaterial.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif



Box2dTest test;
IrrlichtDevice* device = 0;
s32  FireWorkShaderCallBack::gFireWorkMaterialType=0;
bool useWhite;

#define PARTICLE_POOL_SIZE 10
#define RENDER_TARGET1_SIZE (256)

Rocket::Core::Context* context = NULL;
ISceneNode* skybox;
f32 gNodeElapsedTime;

f32 GetGlobalNodeElapsedTime(void)
{
	return gNodeElapsedTime;
}

void SetGlobalNodeElapsedTime(f32 value)
{
	gNodeElapsedTime= value;
}


class EventManager : public IEventReceiver
{

private:
    // We use this array to store the current state of each key
    bool KeyIsDown[KEY_KEY_CODES_COUNT];
	
	
	b2Vec2 mDifXY;
	ISceneManager* mSmgr;
	bool firstTime;

public:
	b2Vec2 mBoxWorldXY;
    struct SMouseState
    {
        core::position2di Position;
        bool LeftButtonDown;
        SMouseState() : LeftButtonDown(false) { }
    } MouseState;

	 void CalculateWorldXY(bool reset = false, bool touch=false)
	 {
		
		core::line3df ray = 
				mSmgr->getSceneCollisionManager()->getRayFromScreenCoordinates(MouseState.Position, mSmgr->getActiveCamera());

		core::plane3df plane(core::vector3df(0, 0, 0), core::vector3df(0, 0, -1));

        core::vector3df mouseWorldPosition;

        if(plane.getIntersectionWithLine(ray.start, ray.getVector(), mouseWorldPosition))
		{			

			
			if(reset)
			{

				mDifXY = b2Vec2(mouseWorldPosition.X,mouseWorldPosition.Y) - mBoxWorldXY;
				firstTime=true;
			}			
			
			{
				mBoxWorldXY.x = mouseWorldPosition.X; //- mDifXY.x;
				mBoxWorldXY.y = mouseWorldPosition.Y;
			}
		}

	 };


    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        // Remember whether each key is down or up
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
		{
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

			if(event.KeyInput.PressedDown)
			{
				test.Keyboard(event.KeyInput.Key);			
			}
			else
			{
				test.KeyboardUp(event.KeyInput.Key);
			}
		}
		else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
			
            switch(event.MouseInput.Event)
            {
            case EMIE_LMOUSE_PRESSED_DOWN:
				{
					
					MouseState.Position.X = event.MouseInput.X;
					MouseState.Position.Y = event.MouseInput.Y;
					CalculateWorldXY(true,true);


					if(!MouseState.LeftButtonDown)
					{
						test.MouseDown(mBoxWorldXY);
					}
					MouseState.LeftButtonDown = true;
			
					break;
				}
            case EMIE_LMOUSE_LEFT_UP:
				{
					MouseState.LeftButtonDown = false;
					test.MouseUp(mBoxWorldXY);

					

					break;
				}
            case EMIE_MOUSE_MOVED:
				{
	                
					//transform xy to world xy
					MouseState.Position.X = event.MouseInput.X;
					MouseState.Position.Y = event.MouseInput.Y;
					CalculateWorldXY(false, MouseState.LeftButtonDown);
					
										


					test.MouseMove(mBoxWorldXY);
					break;
			}
            default:
				{
                // We won't use the wheel
                break;
				}
			}
        }

        return false;
    }



    // This is used to check whether a key is being held down
    virtual bool IsKeyDown(EKEY_CODE keyCode) const
    {
        return KeyIsDown[keyCode];
    }
    
	EventManager():mSmgr(NULL)
    {
        for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
            KeyIsDown[i] = false;

		firstTime=false;
		mBoxWorldXY.x=0; 
		mBoxWorldXY.y=0;
    }

	void SetSceneManager(ISceneManager* smgr)
	{
		 mSmgr=smgr;
	}


};

/*
This is the main method. We can now use main() on every platform.
*/

#ifdef __cplusplus
   extern "C"
 #endif
int main()
{
	//VLDEnable(); //memory leak debugger

	EventManager eventMgr;

	device = createDevice( video::EDT_OGLES2, dimension2d<u32>(640, 480), 16, false, false, false, &eventMgr);

	if (!device)
		return 1;


	device->sleep(1000);


	device->setWindowCaption(L"Box2d test");


	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	
	eventMgr.SetSceneManager(smgr);
	device->getFileSystem()->changeWorkingDirectoryTo(".");
	
	video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	
	if (gpu)
	{	
		FireWorkShaderCallBack::Initialize(device,GetGlobalNodeElapsedTime);
	}


	ICameraSceneNode* activeCamera = smgr->addCameraSceneNode(0, vector3df(0,-2,-7), vector3df(0,0,0));


	//setup world simulation
	test.Initialize(smgr);


	Settings testSettings;
	int lastFPS = -1;
	u32 timeStep = 16; // 25 milliseconds == 40 FPS.
	
	//Setup firework node
	FireworkNode::InitialzeVBO(64,64,smgr);

	// create render target
	video::ITexture* renderTarget1 = 0;
	irr::scene::ICameraSceneNode* target1Camera  = NULL;
	
	SColor rtColors[4];
	rtColors[0]=SColor(255,255,255,255);
	rtColors[1]=SColor(255,255,255,255);
	rtColors[2]=SColor(255,255,255,255);
	rtColors[3]=SColor(255,255,255,255);

	
	//setup irr rocket
	CIrrRocketGUI rocketGUI(device);

	
	//initialize Audio

	AudioManager* audiomgr = AudioManager::getInstance();
	if(audiomgr) 	
	{
		if (!audiomgr->initialize())
			return 0;
	}		

	audiomgr->playMusic("IrrlichtSdcard/backmusic3.ogg");
 
	AudioItem audioPull;


	int particleIds[PARTICLE_POOL_SIZE];
	int particleID = 1000000;
	int levelUpCounter=0;
	int counterReason=0;

	for(int i=0;i<PARTICLE_POOL_SIZE;i++)
		particleIds[i]=0;



	particleID++;

	
	BatchedSpriteNode* batchedNode = new BatchedSpriteNode
									(smgr->getRootSceneNode(), 
									smgr,
									particleID,
									500);
	batchedNode->Load("IrrlichtSdcard/figures.tga",128,128);	
	

	//Create skybox
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
    skybox=smgr->addSkyBoxSceneNode(
	driver->getTexture("IrrlichtSdcard/skyboxspace.tga"),
	NULL,
	NULL,
	NULL,
	NULL,
	NULL);
    driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);
	scene::ISceneNodeAnimator* anim = smgr->createRotationAnimator(core::vector3df(0.00f,0.02f,0.0));
	skybox->setRotation(core::vector3df(-10.0f,0.0f,0.0f));
    if(skybox)
    {
        skybox->addAnimator(anim);
	}
	

	test.SetGamePlayMode(Box2dTest::GPM_BREAKOUT);

	while(device->run())
	{

		u32 startTime = device->getTimer()->getRealTime();		
	
		test.Step(&testSettings);

		batchedNode->setGameObjects(&test.mGameObjects);
		
		for(int i=0;i<PARTICLE_POOL_SIZE;i++)
		{
			int nodeId= particleIds[i];
			if(nodeId !=0)
			{
				FireworkNode* fireWorkNode = (FireworkNode*)smgr->getSceneNodeFromId(nodeId);

				if(fireWorkNode)
				{
					if(fireWorkNode->getLife()<=0)
					{
						fireWorkNode->remove();
						particleIds[i]=0;
					}
					else
					{
						fireWorkNode->UpdateTime(startTime);
					}
				}
				else
				{
					particleIds[i]=0;
				}
			}
		}

		
		if(test.hasCollision || test.mCacheData.size()>0)
		{
			
			for (irr::u32 j = 0; j < test.mCacheData.size(); j++)
			{
				

				for(int i=0;i<PARTICLE_POOL_SIZE;i++)
				{
					if(particleIds[i]==0)
					{
						particleID++;
						FireworkNode* fireWorkNode = new FireworkNode(smgr->getRootSceneNode(), smgr,particleID,startTime,0.2f);						
						fireWorkNode->setMaterialType((video::E_MATERIAL_TYPE)FireWorkShaderCallBack::gFireWorkMaterialType);					
						fireWorkNode->setPosition(vector3df(
							test.mCacheData[j].cachedCenter.x,
							test.mCacheData[j].cachedCenter.y,
							0));
						fireWorkNode->setLife(500);
						fireWorkNode->setVisible(true);						
						particleIds[i]= particleID;
						break;
					}
				}

			}
		}
		
		audiomgr->update();



		
		
		driver->beginScene(true, true, SColor(255,0,0,0));
		
		smgr->setActiveCamera(activeCamera);		
		smgr->drawAll();

		float score=  test.player.getScore();
		rocketGUI.SetScore((int)(score*100));
		rocketGUI.SetWave(lastFPS);
		rocketGUI.run();

		int fps = driver->getFPS();

        if (lastFPS != fps)
        {
			lastFPS = fps;
        }

		
		driver->endScene();
		

		//LOCK 60fps in win32
		u32 timeTaken = device->getTimer()->getRealTime()-startTime;
		if (timeTaken < timeStep)
		{
			device->sleep(timeStep - timeTaken);
		}
	}

	device->drop();
	
	if(audiomgr)	
		audiomgr->shutDown();

	return 0;
}

