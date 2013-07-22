// Copyright (C) 2002-2009 Laurent Mallet & Maxime Biais
// This file is part of the "Irrlicht Android Port".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include <jni.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>
#include <stdint.h>
#include <Box2D/Box2D.h>
#include <importgl.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include <irrlicht.h>
#include "COGLES2Driver.h"
// needed here also because of the create methods' parameters
#include "CNullDriver.h"
#include <CIrrDeviceStub.h>
#include <os.h>
#include <Rocket/Core.h>
#include "RocketInterface/CIrrRocketGUI.h"
#include "Audio/AudioManager.h"
#include "Base2dNode.h"
#include "B2dNode.h"
#include "SpriteNode.h"
#include "FireworkNode.h"
#include "GameObject.h"
#include "BatchedSpriteNode.h"
#include "Player.h"
#include "Box2dTest.h"

using namespace irr;
using namespace os;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
#include "Materials/ClearMaterial.h"
#include "Materials/FireworkMaterial.h"
#include <android-receiver.h>
#include <android-logger.h>



extern IrrlichtDevice *device;
extern IVideoDriver* driver;
extern stringc gSdCardPath;

#define PARTICLE_POOL_SIZE 10
#define RENDER_TARGET1_SIZE (256)

int particleIds[PARTICLE_POOL_SIZE];

int particleID = 1000000;


AudioManager* audiomgr;
ISceneManager* smgr = NULL;
IGUIEnvironment* guienv = NULL;
IAnimatedMesh* mesh = NULL;
ISceneNode* node = NULL;
IAnimatedMeshSceneNode *nodeSydney = NULL;
ICameraSceneNode* camera = NULL;
ICameraSceneNode* activeCamera = NULL;
IGUIStaticText* diagnostics = NULL;
SAppContext context;
AndroidEventReceiver *receiver;


//Create a render target (FBO) with plain orthogonal projection
//we will do the motion blur effect within this fbo
//and then render it as a plain texture
video::ITexture* renderTarget1 = 0;
irr::scene::ICameraSceneNode* target1Camera  = NULL;
SpriteNode* darkNode = NULL;
SpriteNode* blurNode= NULL;


int particleFadeCount=30;
bool demoInit = false;
AudioItem audioPull;


BatchedSpriteNode* batchedNode;

CIrrRocketGUI* rocketGUI;
ISceneNode* skybox;
Box2dTest* test = NULL;

s32  ClearMaterialCallBack::gClearMaterialType=0;
s32  FireWorkShaderCallBack::gFireWorkMaterialType=0;
#define RENDER_TARGET_SIZE_X 1024
#define RENDER_TARGET_SIZE_Y 1024

bool useWhite;




static long long counter = 0;



f32 gNodeElapsedTime;

f32 GetGlobalNodeElapsedTime(void)
{
	return gNodeElapsedTime;
}

void SetGlobalNodeElapsedTime(f32 value)
{
	gNodeElapsedTime= value;
}

static long _getTime(void)
{
    struct timeval  now;

    gettimeofday(&now, NULL);
    return (long)(now.tv_sec*1000 + now.tv_usec/1000);
}

void CreateMaterialRenderer()
{
	video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();	
	if (gpu)
	{
		__android_log_print(ANDROID_LOG_INFO, "DemoTest", "gpu services ok");
		FireWorkShaderCallBack::Initialize(device,GetGlobalNodeElapsedTime);
		ClearMaterialCallBack::Initialize(device);
	}
}

void CreateRenderTarget()
{
	if (driver->queryFeature(video::EVDF_RENDER_TO_TARGET))
	{

		//we need an offscreen surface with alpha channel
		//not all GPUs support 8888, must do a work around
		renderTarget1 = driver->addRenderTargetTexture(core::dimension2d<u32>(RENDER_TARGET1_SIZE,RENDER_TARGET1_SIZE), "RTT1", ECF_A8R8G8B8);
		target1Camera = device->getSceneManager()->addCameraSceneNode(
						0, 
						irr::core::vector3df(0.0f, 
											 0.0f, 
										    -2.0f),
						irr::core::vector3df(0.0f, 
											 0.0f, 
											 0.0f),
						 -1,
						false);
		irr::core::matrix4 mat;
				mat.buildProjectionMatrixOrthoLH(
					10.0f, 

					10.0f,
					-0.1f,10.0f);
				target1Camera->setProjectionMatrix(mat,true);
							

	}
	
}

bool initDemo()
{	

	audioPull.mId =-1;

    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
 
	
    stringc myDir = gSdCardPath;
    myDir += "/GameDemo";
    device->getFileSystem()->changeWorkingDirectoryTo(myDir.c_str());
 	
	
	audiomgr = AudioManager::getInstance();

	if(audiomgr) 	
	{
		if (audiomgr->initialize() < 0)
		{
			return false;
		}
	}		

	CreateMaterialRenderer();

	CreateRenderTarget();



	rocketGUI= new CIrrRocketGUI(device);

	__android_log_print(ANDROID_LOG_INFO, "Demo", "Initializing box2dTest");

	test = new Box2dTest();
	test->Initialize(smgr);

	context.smgr  = smgr;
	context.device = device;
	context.test = test;
	

	//Setup firework node
	FireworkNode::InitialzeVBO(64,64,smgr);
	
	activeCamera = smgr->addCameraSceneNode(0, vector3df(0,-2,-7), vector3df(0,0,0));

	// Then create the event receiver, giving it that context structure.
    receiver = new AndroidEventReceiver(context);

    // And tell the device to use our custom event receiver.
    device->setEventReceiver(receiver);




	for(int i=0;i<PARTICLE_POOL_SIZE;i++)
		particleIds[i]=0;


	
	particleID++;

	batchedNode = new BatchedSpriteNode
					(smgr->getRootSceneNode(), 
						smgr,
						particleID,
						500);
	batchedNode->Load("/mnt/sdcard/GameDemo/figures.tga",128,128);
	
	

	particleID++;
	
	//Create skybox
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
    skybox=smgr->addSkyBoxSceneNode(
	driver->getTexture("/mnt/sdcard/GameDemo/skyboxspace.tga"),
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

	particleID++;

	//transparent dark texture to apply each frame over the offscreen render target 
	//to create the blur effect
 	darkNode = new SpriteNode(smgr->getRootSceneNode(), smgr,particleID);
    darkNode->Load("/mnt/sdcard/GameDemo/cleartexture.tga",128,128,10.0f,10.0f);
	darkNode->setMaterialType((video::E_MATERIAL_TYPE)ClearMaterialCallBack::gClearMaterialType);
	darkNode->setPosition(vector3df(0.0f,0.0f,-0.5f));
    darkNode->setFrame(0);

	particleID++;


	//Create an sprite node using the offscreen render target as its texture
	blurNode= new SpriteNode(smgr->getRootSceneNode(), smgr,particleID);
	blurNode->LoadFromTexture(renderTarget1,RENDER_TARGET1_SIZE,
											RENDER_TARGET1_SIZE,10.0f,10.0f);
	blurNode->setMaterialType((video::E_MATERIAL_TYPE)ClearMaterialCallBack::gClearMaterialType);
	blurNode->setTransparent(true);
	blurNode->setPosition(vector3df(0.0f,0.0f,-0.1f));



	test->SetGamePlayMode(Box2dTest::GPM_BREAKOUT);


	return true;
}

/* Call to render the next GL frame */
void nativeDrawIterationDemo()
{	
	Settings testSettings;
	int lastFPS = -1;
	u32 timeStep = 16; // 25 milliseconds == 40 FPS.

    if (counter==0) 
	{
		__android_log_print(ANDROID_LOG_INFO, "DemoTest", "to init demo");
		demoInit = initDemo();

		if(demoInit)
			__android_log_print(ANDROID_LOG_INFO, "DemoTest", "to init demo success");
		else
			__android_log_print(ANDROID_LOG_INFO, "DemoTest", "to init demo fail");

		audiomgr->playMusic("/mnt/sdcard/GameDemo/backmusic3.ogg");

    }

	if (device->run()&& demoInit)
	{
		((CIrrDeviceStub*) device)->dispatchEventsFromUser();

		u32 startTime = device->getTimer()->getRealTime();		
		test->Step(&testSettings); //run simulation

		batchedNode->setGameObjects(&test->mGameObjects);
	
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

	
		if(receiver && test->hasCollision || test->mCacheData.size()>0 )
		{
					
			for (irr::u32 j = 0; j < test->mCacheData.size(); j++)
			{
				for(int i=0;i<PARTICLE_POOL_SIZE;i++)
				{
					if(particleIds[i]==0)
					{
						particleID++;
						FireworkNode* fireWorkNode = new FireworkNode(smgr->getRootSceneNode(), smgr,particleID,startTime,0.2f);
						fireWorkNode->setMaterialFlag(video::EMF_LIGHTING, false);
						fireWorkNode->setMaterialType((video::E_MATERIAL_TYPE)FireWorkShaderCallBack::gFireWorkMaterialType);					
						fireWorkNode->setRotation(vector3df(0.0f,0.0f,(float)(rand()%360)));
						fireWorkNode->setPosition(vector3df(
							test->mCacheData[j].cachedCenter.x,
							test->mCacheData[j].cachedCenter.y,
							0));
						fireWorkNode->setLife(500);						
						particleIds[i]= particleID;
						break;
					}
				}
			}
		}
		
		audiomgr->update();

		driver->beginScene(true, true, 0);


		//render to the fbo apply motion blur to the balls
		if (renderTarget1)
		{

			//make invisible all objects excluding the ball & paddle
			for(int i=0;i<test->mGameObjects.size();i++)
			{
				test->mGameObjects[i].mVisible =  (test->mGameObjects[i].mUserData->m_shapeType == 0);
			}

			//Start drawing to the offscreen buffer 
			driver->setRenderTarget(renderTarget1, counter==0, false, video::SColor(0,0,0,0));
			darkNode->setVisible(true);
			target1Camera->render();
			darkNode->render();
			batchedNode->render();

			//Switch back to the screen buffer
			driver->setRenderTarget(0, true, true, 0);		

		}
			
		darkNode->setVisible(false);
		//make visible all objects again 
		for(int i=0;i<test->mGameObjects.size();i++)
		{
				test->mGameObjects[i].mVisible = true;
		}


		smgr->setActiveCamera(activeCamera);
		smgr->drawAll();
		
		float score=  (float)test->player.getScore();
		
		rocketGUI->SetScore((int)(score*100));
		rocketGUI->SetWave(lastFPS);
		rocketGUI->run();

		int fps = driver->getFPS();

		if (lastFPS != fps)
		{
			lastFPS = fps;
		}
		driver->endScene();

			
		u32 timeTaken = device->getTimer()->getRealTime()-startTime;

		if (timeTaken < timeStep)
		{
		//	device->sleep((timeStep - timeTaken)/2);
		}
		
	}

	counter++ ;
}


void nativeDrawIteration()
{
    nativeDrawIterationDemo();
	
}

