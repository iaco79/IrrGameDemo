#include <cstdio>
#include "SDL.h"
#include "SDL_mixer.h"
#include <irrlicht.h>
#include <Box2D/Box2D.h>
#include "Base2dNode.h"
#include "B2dNode.h"
#include "SpriteNode.h"
#include "GameObject.h"
#include "Player.h"
#include "Box2dTest.h"
#include "Audio/AudioManager.h"

#include <new>

#if defined(_IRR_ANDROID_PLATEFORM_)
#include <android/log.h>
#endif

using namespace std;
using namespace irr;

#ifndef GROUND_ID

	#define GROUND_ID 2000
#endif


#ifndef GAMEOBJECT_ID 
 #define GAMEOBJECT_ID  20000
#endif 
#ifndef COLLIDE_CIRCLE_NODE_ID 
	#define COLLIDE_CIRCLE_NODE_ID 2002
#endif

#ifndef BOX_SIZE
#define BOX_SIZE (10.0f/2.0f)
#endif


#ifndef CIRCLE_SEGMENTS
#define CIRCLE_SEGMENTS 8
#endif


#ifndef SHAPE_FACTOR
#define SHAPE_FACTOR 0.5f
#endif

#ifndef COLLIDE_TIMER
#define COLLIDE_TIMER 15
#endif

#define	SHAPE_BACKGROUND (FIGUREDEF_COUNT + 1)
#define MAX_COLLIDE_SOUNDS 4

const int CATEGORY_GROUND=1;
const int CATEGORY_FIGURE=2;

const int MASK_GROUND= -1;
const int MASK_FIGURE = CATEGORY_FIGURE |  CATEGORY_GROUND;


long long frameCount=0;
long long frameCountLevel=0;
int  maxFigures=0;


int Box2dTest::WebTest[400] = {
							 43,00,50,00,50,00,50,00,50,00,50,00,50,00,50,00,50,00,42,00,
							 00,50,00,50,00,50,00,50,00,50,00,50,00,50,00,50,00,50,00,00,
							 51,00,20,00,20,00,20,00,20,00,20,00,20,00,20,00,20,00,20,53,
							 00,20,00,20,00,20,00,20,00,20,00,20,00,20,00,20,00,20,00,00,
							 51,00,20,00,20,00,20,00,20,00,20,00,20,00,20,00,20,00,20,53,
							 00,20,00,20,00,20,00,20,00,20,00,20,00,20,00,20,00,20,00,00,
							 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
							 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
							 00,00,41,00,50,00,50,00,50,00,50,00,50,00,50,00,40,00,00,00,
							 00,00,00,00,33,00,00,00,00,00,00,00,00,00,00,32,00,00,00,00,
							 00,00,51,33,00,00,00,00,00,00,00,00,00,00,00,00,32,53,00,00,
							 00,00,00,00,00,00,00,00,00,50,00,00,00,00,00,00,00,00,00,00,
							 00,00,51,00,00,00,31,20,00,00,00,00,20,30,00,00,00,53,00,00,
							 00,00,00,00,00,31,00,00,00,00,00,00,00,00,30,00,00,00,00,00,
							 00,00,51,00,31,00,00,00,00,00,00,00,00,00,00,30,00,53,00,00,
							 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
							 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,							 
							 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
							 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
							 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00							 
							};

Box2dTest::Box2dTest()
{
	//mCollideTimer = COLLIDE_TIMER;
	mGamePlayMode = GPM_NONE;
	
	
	mCollideExternal=0;	
	mIdCounter=0;    
	mPaddle = NULL;	
	b2Vec2 gravity;
	//gravity.Set(0.0f, -1.0f);	
	gravity.Set(0.0f, 0.0f);	

	m_world = new b2World(gravity, false);	
	m_world->SetContactListener(this);

	
	mIacoPoint = b2Vec2(0.0f,0.0f);
	mJointCount=0;

	for (int i = 0; i < 256; i++)
	{
		m_joints[i]= NULL;

	}



}

void Box2dTest::SetupGround()
{

	b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0,0);
    b2Body* groundBody = m_world->CreateBody(&groundBodyDef);

    b2EdgeShape groundEdge;
    b2FixtureDef boxShapeDef;
    boxShapeDef.shape = &groundEdge;
	
	boxShapeDef.density = 1.0f;
	boxShapeDef.friction = 0.05f;
	boxShapeDef.restitution = 1.0f;

	boxShapeDef.filter.categoryBits = CATEGORY_GROUND;
	boxShapeDef.filter.maskBits  = MASK_GROUND;

    
    groundEdge.Set(b2Vec2(-BOX_SIZE,-BOX_SIZE), b2Vec2(-BOX_SIZE,BOX_SIZE));
    groundBody->CreateFixture(&boxShapeDef);

    
    groundEdge.Set(b2Vec2(BOX_SIZE,BOX_SIZE), b2Vec2(BOX_SIZE,-BOX_SIZE));
    groundBody->CreateFixture(&boxShapeDef);
	

	//groundEdge.Set(b2Vec2(-BOX_SIZE,-BOX_SIZE), b2Vec2(BOX_SIZE, -BOX_SIZE));
    //groundBody->CreateFixture(&boxShapeDef);

	groundEdge.Set(b2Vec2(-BOX_SIZE,BOX_SIZE), b2Vec2(BOX_SIZE,BOX_SIZE));
    groundBody->CreateFixture(&boxShapeDef);


	//create irrlich node
	int id = GROUND_ID;
	video::SColor scolor(255,128,0,255);

	B2dNode* node = new B2dNode(mSmgr->getRootSceneNode(), mSmgr,id, groundBody, scolor);
	node->setVisible(true);

	BoxData* boxData = new BoxData();

	boxData->m_shapeType= SHAPE_BACKGROUND;

	
	mGroundObject  = GameObject(boxData);
	mGroundObject.mBody = groundBody;
	groundBody->SetUserData(mGroundObject.mUserData);
	mGroundObject.mNodeId = id;
//	mGroundObject.mBody->SetUserData(NULL);
	


}

void Box2dTest::SetupShapes()
{	
	E_FIGUREDEF index=FIGUREDEF_BALL;

	//ball
	{	
		index = FIGUREDEF_BALL;

		b2Vec2 vertices[CIRCLE_SEGMENTS];
		for(int i=0;i<CIRCLE_SEGMENTS;i++)
		{
			vertices[i].Set(0.25f*SHAPE_FACTOR * cos(2*irr::core::PI/CIRCLE_SEGMENTS*i), 
							0.25f*SHAPE_FACTOR * sin(2*irr::core::PI/CIRCLE_SEGMENTS*i));
		}

		
		m_figureDefs[index].shape.Set(vertices, CIRCLE_SEGMENTS);
		m_figureDefs[index].figureType = index;
		m_figureDefs[index].width =  SHAPE_FACTOR*1.2f;
		m_figureDefs[index].height = SHAPE_FACTOR*1.2f;
		m_figureDefs[index].boxWidth   = 0.5f*SHAPE_FACTOR;
		m_figureDefs[index].boxHeight  = 0.5f*SHAPE_FACTOR;		
		m_figureDefs[index].textX0 = 0;
		m_figureDefs[index].textY0 = 0;
		m_figureDefs[index].textX1 = 127;
		m_figureDefs[index].textY1 = 127;		
		m_figureDefs[index].friction =  0.0f;
		m_figureDefs[index].restitution = 1.0f;
		m_figureDefs[index].density = 1.0f;



	}

	//box
	{
		index = FIGUREDEF_BRICK_BOX;
		m_figureDefs[index].shape.SetAsBox(0.5f*SHAPE_FACTOR, 0.5f*SHAPE_FACTOR);
		m_figureDefs[index].figureType = index;
		m_figureDefs[index].width = SHAPE_FACTOR*2.2f;
		m_figureDefs[index].height = SHAPE_FACTOR*2.2f;
		m_figureDefs[index].boxWidth   = SHAPE_FACTOR;
		m_figureDefs[index].boxHeight  = SHAPE_FACTOR;
		m_figureDefs[index].textX0 = 128;
		m_figureDefs[index].textY0 = 0;
		m_figureDefs[index].textX1 = 255;
		m_figureDefs[index].textY1 = 127;
		m_figureDefs[index].friction =  0.02f;
		m_figureDefs[index].restitution = 1.0f; 
		m_figureDefs[index].density = 1.0f;

	}
	//small Triangle
	{
		index = FIGUREDEF_BRICK_TRIANGLE;

		b2Vec2 vertices[3];		
		float radian = 0.0f;	
		vertices[0].Set(-0.5f*SHAPE_FACTOR ,  0.5f*SHAPE_FACTOR);
		vertices[1].Set(-0.5f*SHAPE_FACTOR , -0.5f*SHAPE_FACTOR);
		vertices[2].Set( 0.5f*SHAPE_FACTOR , -0.5f*SHAPE_FACTOR);
		m_figureDefs[index].shape.Set(vertices,3);
		m_figureDefs[index].figureType = index;
		m_figureDefs[index].width =  SHAPE_FACTOR*1.5f;
		m_figureDefs[index].height = SHAPE_FACTOR*1.5f;
		m_figureDefs[index].boxWidth   = SHAPE_FACTOR;
		m_figureDefs[index].boxHeight  = SHAPE_FACTOR;
		m_figureDefs[index].textX0 = 256;
		m_figureDefs[index].textY0 = 0;
		m_figureDefs[index].textX1 = 383;
		m_figureDefs[index].textY1 = 127;
		m_figureDefs[index].friction =  0.02f;
		m_figureDefs[index].restitution = 1.0f; 
		m_figureDefs[index].density = 1.0f;
	}

	//small Triangle
	{
		index = FIGUREDEF_BRICK_TRIANGLE_BIG;

		b2Vec2 vertices[3];		
		float radian = 0.0f;	
		vertices[0].Set(-SHAPE_FACTOR ,  SHAPE_FACTOR);
		vertices[1].Set(-SHAPE_FACTOR , -SHAPE_FACTOR);
		vertices[2].Set( SHAPE_FACTOR , -SHAPE_FACTOR);
		m_figureDefs[index].shape.Set(vertices,3);
		m_figureDefs[index].figureType = index;
		m_figureDefs[index].width =  SHAPE_FACTOR*2.5f;
		m_figureDefs[index].height = SHAPE_FACTOR*2.5f;
		m_figureDefs[index].boxWidth   = 2.0f*SHAPE_FACTOR;
		m_figureDefs[index].boxHeight  = 2.0f*SHAPE_FACTOR;
		m_figureDefs[index].textX0 = 256;
		m_figureDefs[index].textY0 = 0;
		m_figureDefs[index].textX1 = 383;
		m_figureDefs[index].textY1 = 127;
		m_figureDefs[index].friction =  0.02f;
		m_figureDefs[index].restitution = 1.0f; //SHAPE_RESTITUTION;;
		m_figureDefs[index].density = 1.0f;
	}
	//rectangle
	{
		index = FIGUREDEF_BRICK_RECTANGLE;
		m_figureDefs[index].shape.SetAsBox(SHAPE_FACTOR, 0.5f*SHAPE_FACTOR);
		m_figureDefs[index].figureType = index;
		m_figureDefs[index].width = SHAPE_FACTOR*2.6f;
		m_figureDefs[index].height = SHAPE_FACTOR*2.0f;
		m_figureDefs[index].boxWidth   = 2.0f*SHAPE_FACTOR;
		m_figureDefs[index].boxHeight  = SHAPE_FACTOR;
		m_figureDefs[index].textX0 = 0;
		m_figureDefs[index].textY0 = 128;
		m_figureDefs[index].textX1 = 255;
		m_figureDefs[index].textY1 = 255;
		m_figureDefs[index].friction =  0.02f;
		m_figureDefs[index].restitution = 1.0f; 
		m_figureDefs[index].density = 1.0f;

	}
	//Paddle
	{
		index = FIGUREDEF_PADDLE;

		b2Vec2 vertices[6];		
		float radian = 0.0f;	
		vertices[0].Set(-1.5f*SHAPE_FACTOR ,  0.5f*SHAPE_FACTOR);
		vertices[1].Set(-2.5f*SHAPE_FACTOR ,  0.0f*SHAPE_FACTOR);
		vertices[2].Set(-2.5f*SHAPE_FACTOR , -0.5f*SHAPE_FACTOR);

		vertices[3].Set( 2.5f*SHAPE_FACTOR , -0.5f*SHAPE_FACTOR);
		vertices[4].Set( 2.5f*SHAPE_FACTOR ,  0.0f*SHAPE_FACTOR);
		vertices[5].Set( 1.5f*SHAPE_FACTOR ,  0.5f*SHAPE_FACTOR);

		m_figureDefs[index].shape.Set(vertices,6);
		m_figureDefs[index].figureType = index;
		m_figureDefs[index].width = SHAPE_FACTOR*6.4f;
		m_figureDefs[index].height = SHAPE_FACTOR*2.2f;
		m_figureDefs[index].boxWidth   = 5.0f*SHAPE_FACTOR;
		m_figureDefs[index].boxHeight  = SHAPE_FACTOR;
		m_figureDefs[index].textX0 = 0;
		m_figureDefs[index].textY0 = 256;
		m_figureDefs[index].textX1 = 255;
		m_figureDefs[index].textY1 = 383;
		m_figureDefs[index].friction =  0.02f;
		m_figureDefs[index].restitution = 1.0f; //SHAPE_RESTITUTION;;
		m_figureDefs[index].density = 1.0f;
	}
}



void Box2dTest::SetupCollisionBody()
{
	if(!mPaddle)
	{
		mIdCounter++;
		int id = GAMEOBJECT_ID+mIdCounter;	
		int index = FIGUREDEF_PADDLE;
	
		BoxData* boxData   = new BoxData();
		boxData->m_shapeType=index;			
		mPaddle  = new GameObject(boxData);

		b2BodyDef bd;		
		bd.type = b2_staticBody;
		bd.position.Set(0.0f,-4.0f);	
	
		mPaddle->mBody = m_world->CreateBody(&bd);		 	
		b2FixtureDef fd;
		fd.shape = &(m_figureDefs[index].shape);
		fd.density = m_figureDefs[index].density;
		fd.friction = m_figureDefs[index].friction;
		fd.restitution = m_figureDefs[index].restitution;
		fd.filter.categoryBits = CATEGORY_FIGURE;
		fd.filter.maskBits = MASK_FIGURE;
		mPaddle->mBody->CreateFixture(&fd);
		mPaddle->mSpriteFrame = 0;
		mPaddle->mWidth = m_figureDefs[index].width;
		mPaddle->mHeight = m_figureDefs[index].height;
		mPaddle->mTextX0 = m_figureDefs[index].textX0;
		mPaddle->mTextY0 = m_figureDefs[index].textY0;
		mPaddle->mTextX1 = m_figureDefs[index].textX1;
		mPaddle->mTextY1 = m_figureDefs[index].textY1;
		mPaddle->mBoxHeight = m_figureDefs[index].boxHeight;
		mPaddle->mBoxWidth = m_figureDefs[index].boxWidth ;
		mPaddle->mNodeId = id;
		mPaddle->mBody->SetUserData(mPaddle->mUserData);			
		mPaddle->mActive=true;

		SpriteNode* node = new SpriteNode(mSmgr->getRootSceneNode(), mSmgr,id);				

		#if defined(_IRR_ANDROID_PLATEFORM_)
			node->Load("/mnt/sdcard/GameDemo/Figures.tga",256,128,
				m_figureDefs[index].width,
				m_figureDefs[index].height
				);
		#else
			node->Load("IrrlichtSdcard/Figures.tga",256,128,
				m_figureDefs[index].width,
				m_figureDefs[index].height);
		#endif

		node->setFrame(4);
	}

}	




void Box2dTest::Initialize(irr::scene::ISceneManager* smgr)
{
	mSmgr = smgr;
	SetupGround();
	SetupShapes();
	SetupCollisionBody();
	
}

 
Box2dTest::~Box2dTest()
{
	// By deleting the world, we delete the bomb, mouse joint, etc.
	delete m_world;
	m_world = NULL;
}


//iaco end



void Box2dTest::KeyboardUp(irr::EKEY_CODE keycode)
{

    switch (keycode)
    {
		case irr::KEY_KEY_C:
			SetGamePlayMode(Box2dTest::GPM_CLEAR);		
        break;
		case irr::KEY_KEY_W:
			SetGamePlayMode(Box2dTest::GPM_BREAKOUT);
        break;
    }

}


void Box2dTest::Keyboard(irr::EKEY_CODE keycode)
	{
		switch (keycode)
		{
			case irr::KEY_KEY_1:			
			case irr::KEY_KEY_2:		
			case irr::KEY_KEY_3:		
			case irr::KEY_KEY_4:		
			case irr::KEY_KEY_5:		
				{
					float x =  RandomFloat(-BOX_SIZE+SHAPE_FACTOR*2, BOX_SIZE-SHAPE_FACTOR*2);
					float y =  0 - BOX_SIZE / 2.0f;
					float angle = RandomFloat(-180.0f, 180.0f);

					if(mPaddle && mPaddle->mBody)	
						x = mPaddle->mBody->GetPosition().x;

					int nodeId = Create(keycode - irr::KEY_KEY_1,x,y, angle, true);			

					
					for (irr::u32 i = 0; i < mGameObjects.size(); i++)
					{	
						if (mGameObjects[i].mNodeId == nodeId)
						{
							mGameObjects[i].mBody->SetLinearVelocity(b2Vec2(0.0,RandomFloat(5.0,8.0)));
							break;
						}
					}
				}
			break;					
		}
	}

void Box2dTest::MouseDown(const b2Vec2& p)
{

    mMouseDown = true;
    mIacoPoint = p;      
	mMouseUp=false;
	

}

void Box2dTest::MouseUp(const b2Vec2& p)
{
 
	mMouseUp=true;
    mMouseDown=false;
}

void Box2dTest::MouseMove(const b2Vec2& p)
{

	mIacoPoint = p;
}

void Box2dTest::Step(Settings* settings)
{
	float32 timeStep = settings->hz > 0.0f ? 1.0f / settings->hz : float32(0.0f);

	uint32 flags = 0;
	int collidedCount=0;

	m_world->SetWarmStarting(settings->enableWarmStarting > 0);
	m_world->SetContinuousPhysics(settings->enableContinuous > 0);
	m_world->SetSubStepping(settings->enableSubStepping > 0);

	m_world->Step(timeStep, settings->velocityIterations, settings->positionIterations);
 
	m_world->ClearForces();


	irr::core::array<int> toRemove;	
	mCacheData.clear();
	
	
	for (irr::u32 i = 0; i < mGameObjects.size(); i++)
	{	
		GameObject& gameObject = mGameObjects[i];
		
		if(gameObject.mBody!=NULL)
		{	
			{								
				
				float32 bpx=gameObject.mBody->GetPosition().x;
				float32 bpy=gameObject.mBody->GetPosition().y;
				
				if (bpy < -6.0 && gameObject.mBody->GetLinearVelocity().y <0) 
				{					
					gameObject.mUserData->m_setForRemoval=true;
					gameObject.mUserData->m_withCollision=false;
					
				}
				if (bpy > 6.0  && gameObject.mBody->GetLinearVelocity().y> 0) 
				{
					gameObject.mUserData->m_setForRemoval=true;
					gameObject.mUserData->m_withCollision=false;
						
				}


			

				video::SColorf  clrint(1.0,1.0,1.0,1.0);
				video::SColor  clri;
				
				vector3df scale(1.0f,1.0f,1.0f);

				
				if(gameObject.mUserData->m_emitSound >0)
					gameObject.mUserData->m_emitSound--;

				gameObject.mUserData->m_collideCount=0;
				gameObject.mUserData->m_setContactPointer = false;

					
				if(gameObject.mUserData->m_shapeType< FIGUREDEF_COUNT && gameObject.mUserData->m_setForRemoval ) 
				{
					
					if(gameObject.mUserData->m_withCollision)
					{	
						BodyCacheData cacheData;
						cacheData.cachedShapeType = gameObject.mUserData->m_shapeType;
						cacheData.cachedAngle = gameObject.mBody->GetAngle();
						cacheData.cachedAngulcarVelocity = gameObject.mBody->GetAngularVelocity();
						cacheData.cachedCenter = gameObject.mBody->GetWorldCenter();
						cacheData.cachedLinearVelocity= gameObject.mBody->GetLinearVelocity();
						cacheData.cachedWithCollision = gameObject.mUserData->m_withCollision;						
						m_world->DestroyBody(gameObject.mBody);
						gameObject.mBody=NULL;					
						
						toRemove.push_back(gameObject.mNodeId);										
						mCacheData.push_back(cacheData);
						collidedCount++;
					
					}	
					else
					{
						//objects that go outside the limits
						if(gameObject.mUserData->m_withCollision== false)
						{
							m_world->DestroyBody(gameObject.mBody);
							gameObject.mBody=NULL;											
							toRemove.push_back(gameObject.mNodeId);					
						}

					}
				}	
			}
		}									 	
		
		gameObject.mUserData->m_setForRemoval = false;        	
	}

	//delete collided objects
	for (irr::u32 j = 0; j < toRemove.size(); j++)
	{
		int nodeId = toRemove[j];
		
		for (irr::u32 i = 0; i < mGameObjects.size(); i++)
		{	
			if (mGameObjects[i].mNodeId == nodeId)
			{
				if(mGameObjects[i].mUserData)
				{
					delete mGameObjects[i].mUserData;
				}
				mGameObjects.erase(i);
				break;
			}
		}
	}

	
	player.addToScore(mCacheData.size());

	bool clearWebItem = false;

	SpriteNode* collideBodyNode = (SpriteNode*) mSmgr->getSceneNodeFromId(mPaddle->mNodeId);	

	if(mPaddle->mBody)
	{		
		mPaddle->mBody->SetTransform(b2Vec2(mIacoPoint.x, mPaddle->mBody->GetPosition().y),0);
	
		if(collideBodyNode)
				collideBodyNode->setPosition(
					irr::core::vector3df(
						mPaddle->mBody->GetPosition().x,
						mPaddle->mBody->GetPosition().y,
						0.0f)
						);
	}
	
	frameCount++;
	mCollideExternal=0;
    mMouseUp=false;

}

 void Box2dTest::BeginContact(b2Contact* contact)
  {
	
    void* userData1= contact->GetFixtureA()->GetBody()->GetUserData();
    void* userData2= contact->GetFixtureB()->GetBody()->GetUserData();

    if(userData1 && userData2)
    {		
		BoxData* data1 = (BoxData*)userData1;
        BoxData* data2 = (BoxData*)userData2;
		int sound = -1;

		//if(data1->m_ValidCode == 777777 || data2->m_ValidCode == 777777)
		{
				
		
			{

				if(data1->m_shapeType == FIGUREDEF_BALL && 
				   data1->m_setContactPointer == false 
				   
				   )
				{					
					
					data1->m_setContactPointer=true;

					if(	   data2->m_shapeType == FIGUREDEF_BRICK_BOX
						|| data2->m_shapeType == FIGUREDEF_BRICK_TRIANGLE
						|| data2->m_shapeType == FIGUREDEF_BRICK_TRIANGLE_BIG
						|| data2->m_shapeType == FIGUREDEF_BRICK_RECTANGLE
						
						)
					{
						sound = AudioItem::CHUNK_BLAST;
						data2->m_setForRemoval=true;
					}
					else
					{						
						sound = AudioItem::CHUNK_EXTCOLLIDE;					
					}
					
				}

				if(data2->m_shapeType == FIGUREDEF_BALL && 
				   data2->m_setContactPointer == false )
				
				{					
					
					data2->m_setContactPointer=true;
					
					if(	   data1->m_shapeType == FIGUREDEF_BRICK_BOX
						|| data1->m_shapeType == FIGUREDEF_BRICK_TRIANGLE
						|| data1->m_shapeType == FIGUREDEF_BRICK_TRIANGLE_BIG
						|| data1->m_shapeType == FIGUREDEF_BRICK_RECTANGLE						
						)
					{
						sound = AudioItem::CHUNK_BLAST;
						data1->m_setForRemoval=true;
					}
					else
					{						
						sound = AudioItem::CHUNK_EXTCOLLIDE;					
					}
				}				

				if(mCollideExternal<=MAX_COLLIDE_SOUNDS && sound>-1)
				{	
					AudioManager* audiomgr = AudioManager::getInstance();
					AudioItem audioBlast;
					audioBlast.mChunkId = AudioManager::gChunkIndexes[sound];
					audioBlast.mFlags = AudioItem::PLAYONCE;
					audioBlast.mLoop=0;				
					audioBlast.mId = audiomgr->playSoundNow(audioBlast);						
					mCollideExternal++;					
				}	
			}
		}
    }
}


 int Box2dTest::getFreeJoinIndex()
 {
	int result = -1;

	for(int i=0;i<=mJointCount;i++)
	{
		if (m_joints[i] == NULL)
		{
			result = i;
			break;
		}

	}

	return result;
 }

 void Box2dTest::CreateWeb(int nx,int ny)
 {
	
	float sepX= (BOX_SIZE*2.0f) / (nx);
	float sepY= (BOX_SIZE*2.0f) / (ny);
	
	int sc=0;

	int maxfig = 1+rand()%2+1;
	
	for(int j=0;j<ny;j++)
	{
		for(int i=0;i<nx;i++)
		{
			
			int value = (WebTest[j*nx+i]);
			int index =  (value/10)-1;
			int angle = (value-(index+1)*10)*90.0f;
			

			if(index>=0 && index < FIGUREDEF_COUNT)
			{
				float32 x,y;
				x= -BOX_SIZE +i*sepX;
				y=  BOX_SIZE -j*sepY;
				Create(index, x,y,angle,false, false);
			}
		}
	}	 
 }


int Box2dTest::Create(int index, float x, float y, int angle,bool dynamic, bool center)
{
		mIdCounter++;
		int id = GAMEOBJECT_ID+mIdCounter;
		
		if (index < FIGUREDEF_COUNT)
		{	
			BoxData* boxData   = new BoxData();
			boxData->m_shapeType=index;			
			GameObject gameObject(boxData);

			b2BodyDef bd;		
			bd.type = dynamic?b2_dynamicBody:b2_staticBody;

			if(!center)
			{
				float width  = m_figureDefs[index].boxWidth/ 2.0f;
				float height = m_figureDefs[index].boxHeight  / 2.0f;
				
				if(angle==0 || angle==180)
				{
					bd.position.Set(x+width,y-height);	
				}
				else
				{
					bd.position.Set(x+height,y-width);	
				}
			}
			else
			{
				bd.position.Set(x,y);	
			}
			float fangle = angle*b2_pi / 180.0f;

			bd.angle = fangle ;
			
			gameObject.mBody = m_world->CreateBody(&bd);
		 	
			b2FixtureDef fd;
			fd.shape = &(m_figureDefs[index].shape);
			fd.density = m_figureDefs[index].density;
			fd.friction = m_figureDefs[index].friction;
			fd.restitution = m_figureDefs[index].restitution;
			fd.filter.categoryBits = CATEGORY_FIGURE;
			fd.filter.maskBits = MASK_FIGURE;
			gameObject.mBody->CreateFixture(&fd);
			
			//add to Node List
			gameObject.mSpriteFrame = 0;
			gameObject.mWidth = m_figureDefs[index].width;
			gameObject.mHeight = m_figureDefs[index].height;
			gameObject.mTextX0 = m_figureDefs[index].textX0;
			gameObject.mTextY0 = m_figureDefs[index].textY0;
			gameObject.mTextX1 = m_figureDefs[index].textX1;
			gameObject.mTextY1 = m_figureDefs[index].textY1;
			gameObject.mBoxHeight = m_figureDefs[index].boxHeight;
			gameObject.mBoxWidth = m_figureDefs[index].boxWidth ;
			
			
			mGameObjects.push_back(gameObject);			
			mGameObjects.getLast().mNodeId = id;
			mGameObjects.getLast().mBody->SetUserData((void*)mGameObjects.getLast().mUserData);						
			mGameObjects.getLast().mActive=true;


			return id;
		}	

	return -1;
};

void Box2dTest::ClearAll()
{

	
	irr::core::array<int> toRemove;
	mCacheData.clear();
	maxFigures=0;
	frameCountLevel=0;

	for (irr::u32 i = 0; i < mGameObjects.size(); i++)
	{	
		GameObject& gameObject = mGameObjects[i];
		
		
		if(gameObject.mBody!=NULL)
		{
		
			{	
				BodyCacheData cacheData;
				cacheData.cachedShapeType = gameObject.mUserData->m_shapeType;
				cacheData.cachedAngle = gameObject.mBody->GetAngle();
				cacheData.cachedAngulcarVelocity = gameObject.mBody->GetAngularVelocity();
				cacheData.cachedCenter = gameObject.mBody->GetWorldCenter();
				cacheData.cachedLinearVelocity= gameObject.mBody->GetLinearVelocity();
				cacheData.cachedWithCollision = gameObject.mUserData->m_withCollision;													
				mCacheData.push_back(cacheData);
				
				m_world->DestroyBody(gameObject.mBody);
				gameObject.mBody=NULL;									
				toRemove.push_back(gameObject.mNodeId);
			}
		}
        
		gameObject.mUserData->m_setForRemoval = false;        	
	}

	//delete collided objects
	for (irr::u32 j = 0; j < toRemove.size(); j++)
	{
		int nodeId = toRemove[j];
		
		for (irr::u32 i = 0; i < mGameObjects.size(); i++)
		{	
			if (mGameObjects[i].mNodeId == nodeId)
			{
				if(mGameObjects[i].mUserData)
				{
					delete mGameObjects[i].mUserData;
				}
				mGameObjects.erase(i);
				break;
			}
		}
	}

	mGameObjects.clear();


}

void Box2dTest::DestroyBody()
{

};

irr::scene::ISceneNode* Box2dTest::GetNode(int nodeId)
{
	return mSmgr->getSceneNodeFromId(nodeId);	
}





void Box2dTest::SetGamePlayMode(int mode)
{

	switch(mode)
	{
	case Box2dTest::GPM_CLEAR:
		{
			mGamePlayMode = Box2dTest::GPM_CLEAR;
			maxFigures=0;			
			ClearAll();
		}
		break;
	case Box2dTest::GPM_BREAKOUT:
		{
			mGamePlayMode = Box2dTest::GPM_BREAKOUT;
			maxFigures=0;
			
			ClearAll();
			CreateWeb(20,20);
		}
		break;
	}
}

