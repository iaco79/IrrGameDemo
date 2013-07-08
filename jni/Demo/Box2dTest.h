#ifndef TEST_H
#define TEST_H

#define	RAND_LIMIT	32767
#define	MAX_OBJECTS = 256;


/// Random number in range [-1,1]
inline float32 RandomFloat()
{
	float32 r = (float32)(std::rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = 2.0f * r - 1.0f;
	return r;
}

/// Random floating point number in range [lo, hi]
inline float32 RandomFloat(float32 lo, float32 hi)
{
	float32 r = (float32)(std::rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = (hi - lo) * r + lo;
	return r;
}

/// Test settings. Some can be controlled in the GUI.
struct Settings
{
	Settings() :
		hz(60.0f),
		velocityIterations(6),
		positionIterations(2),
		enableWarmStarting(1),
		enableContinuous(1),
		enableSubStepping(0)
		{};

	
	float32 hz;
	int32 velocityIterations;
	int32 positionIterations;	
	int32 enableWarmStarting;
	int32 enableContinuous;
	int32 enableSubStepping;
};

const int32 k_maxContactPoints = 2048;

struct ContactPoint
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
};



class Box2dTest : public b2ContactListener
{
private:
	
	enum E_FIGUREDEF
	{
		FIGUREDEF_BALL=0,
		FIGUREDEF_BRICK_BOX,
		FIGUREDEF_BRICK_TRIANGLE,
		FIGUREDEF_BRICK_TRIANGLE_BIG,
		FIGUREDEF_BRICK_RECTANGLE,
		FIGUREDEF_PADDLE,
		FIGUREDEF_COUNT
	};

	struct FigureDefinition
	{
		E_FIGUREDEF figureType;
		b2PolygonShape shape;
		float width;
		float height;
		float textX0;
		float textY0;
		float textX1;
		float textY1;
		float friction;
		float restitution;
		float density;
		float boxWidth;
		float boxHeight;
		

	};

	void SetupGround();
	void SetupShapes();
	void SetupCollisionBody();

	static int WebTest[400];
	
public:

	enum GamePlayMode
	{
		GPM_NONE,		
		GPM_BREAKOUT,
		GPM_CLEAR,
		GPM_COUNT	
	};

	b2Joint* m_joints[256];
	int mJointCount;
	int mGamePlayMode;

	

	GameObject mGroundObject;
	//GameObject mCollideCircle;
	//GameObject mCollideBody;	
	
	

	Player player;

	Box2dTest ();
	~Box2dTest ();

	void Initialize(irr::scene::ISceneManager* smgr);
		
	void SetCandyBitsTarget(float x, float y) 
	{
			mCandyBitTarget.Set(x,y);

	}

	irr::scene::ISceneNode* GetNode(int nodeId);

	void Step(Settings* settings);

	void Keyboard(irr::EKEY_CODE key);
	void KeyboardUp(irr::EKEY_CODE key);
	void MouseDown(const b2Vec2& p);
	void MouseUp(const b2Vec2& p);
	void MouseMove(const b2Vec2& p);
		
	void SetGamePlayMode(int mode);

	// b2ContactListener
	void BeginContact(b2Contact* contact); 
		
	int Create(int index, float x, float y, int angle,bool dynamic, bool center = true);
	void CreateWeb(int nx,int ny);
	
	
	void DestroyBody();
	void ClearAll();
	

	

	int getCollideExternal()
	{
		return mCollideExternal;
	
	}
//	int GetCollideTimer() {return mCollideTimer;}
	
	irr::core::array<GameObject> mGameObjects;	
	//irr::core::array<GameObject> mBitObjects;	
	irr::core::array<BodyCacheData> mCacheData;

	b2World* m_world;	
	bool hasCollision;
	
protected:
	
	b2AABB m_worldAABB;	
	
	b2CircleShape circleShapeIacoRadiusForce;

	int mIdCounter;

	int getFreeJoinIndex();    
	int getWaitCount(int playerLevel);
    
    
    
    b2Vec2 mIacoPoint;
	b2Vec2 mCandyBitTarget;
    bool mMouseDown;
	bool mMouseUp;
	

	//IACO end    
	int32 m_bodyIndex;	
	int mCurrentColor;
	int mNewColor;
	//int mCollideTimer;
	
	GameObject* mPaddle;

	int mCollideExternal;
	
	
	irr::scene::ISceneManager* mSmgr;
	

	 FigureDefinition m_figureDefs[FIGUREDEF_COUNT];
    
};

#endif
