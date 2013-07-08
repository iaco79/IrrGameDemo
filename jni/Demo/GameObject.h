//Game objects structures

struct BodyCacheData
{
	
	b2Vec2 cachedCenter;
	float  cachedAngle;
	b2Vec2 cachedLinearVelocity;
	float   cachedAngulcarVelocity;
	int cachedShapeType;
	bool cachedWithCollision;
	int cachedWebIndex;
	int cachedJoinTop;
	int cachedJoinLeft;
	int cachedJoinRight;
	int cachedJoinDown;
};

class BoxData
{
	public:
	BoxData():		
	m_setForRemoval(false),
	m_withCollision(true),
	m_setContactPointer(false),
	m_shapeType(0),
	mRemoveCounter(-1),
	m_emitSound(0),
	m_collideCount(0),
		
	m_webIndex(-1)
	//m_ValidCode(777777) 
	{

	}
	
	bool m_setForRemoval;
	bool m_setContactPointer;
	int  m_shapeType;
	int  m_ValidCode;
	int  m_collideCount;
	int  m_emitSound;
	int  mRemoveCounter;
	bool m_withCollision;
		
	int m_webIndex;
		
		
};
	
class GameObject
{
	public:


	BoxData* mUserData;
	int mNodeId; //Irrlicht Drawable		
	b2Body* mBody; //box 2d Data		
	float mRotation;
	
	int mSpriteFrame;
	bool mActive;
	int mVisible;
	float mScale;
	float mWidth;
	float mHeight;
	float mTextX0;
	float mTextY0;
	float mTextX1;
	float mTextY1;
	float mBoxWidth;
	float mBoxHeight;

	GameObject():		
			mNodeId(0),
			mBody(NULL),
			mUserData(NULL),
			mRotation(0.0f),			
			mActive(true),
			mVisible(true),
			mScale(1.0f),
			mWidth(1.0f),
			mHeight(1.0f),
			mSpriteFrame(0),
			mBoxWidth(0.0f),
			mBoxHeight(0.0f)
	{
		
	};

    GameObject(BoxData*userData) :
			mNodeId(0),
			mBody(NULL),
			mUserData(userData),
			mRotation(0.0f),	
			mActive(true),
			mVisible(true),
			mScale(1.0f),
			mWidth(1.0f),
			mHeight(1.0f),
			mSpriteFrame(0),
			mBoxWidth(0.0f),
			mBoxHeight(0.0f)
    {
		
    };

	// Copy constructor
    GameObject(const GameObject &other)
    {
		mNodeId = other.mNodeId;
		mBody = other.mBody;		
		mUserData = other.mUserData;
		mActive = other.mActive;
		mVisible = other.mVisible;
		mScale  = other.mScale;
		mWidth  = other.mWidth;
		mHeight  = other.mHeight;
		mSpriteFrame = other.mSpriteFrame;
		mTextX0 = other.mTextX0;
		mTextY0 = other.mTextY0;
		mTextX1 = other.mTextX1;
		mTextY1 = other.mTextY1;
		mBoxWidth =  other.mBoxWidth;
		mBoxHeight =  other.mBoxHeight;


    }

	GameObject& operator= (const GameObject& other )
	{
    
		mNodeId = other.mNodeId;
		mBody = other.mBody;		
		mUserData = other.mUserData;		
		mActive = other.mActive;

		mVisible = other.mVisible;
		mScale  = other.mScale;
		mWidth  = other.mWidth;
		mHeight  = other.mHeight;
		mSpriteFrame = other.mSpriteFrame;
		mTextX0 = other.mTextX0;
		mTextY0 = other.mTextY0;
		mTextX1 = other.mTextX1;
		mTextY1 = other.mTextY1;
		mBoxWidth =  other.mBoxWidth;
		mBoxHeight =  other.mBoxHeight;

		// return the existing object
		return *this;
	}

	~GameObject()
	{
			mNodeId=0;
			mBody=NULL;
	}


};