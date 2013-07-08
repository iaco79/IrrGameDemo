//Node to draw several sprites at once

using namespace irr;
using namespace irr::core;
using namespace	irr::video;
using namespace	irr::scene;


class BatchedSpriteNode : public Base2dNode
{
private:

	int mJointCount;
	
	irr::video::S3DVertex*  Vertices;
	u16*		Indices;
    //s32                 stepww,stephh;
	//float fWidth, fHeight;
	int mVertexCount;
	int mIndexCount;
	int mMaxQuads;
	irr::core::array<GameObject>* mGameObjects;

	~BatchedSpriteNode() // destructor
    {		
 
		delete[] Vertices;
		delete[] Indices;

    };

	
public:

	BatchedSpriteNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id, int maxObjects);
	

	void render();

	void setGameObjects(irr::core::array<GameObject>* gameObjects);

	virtual void Load(char* filename,s32 frameWidth,s32 frameHeight)
    {
		IVideoDriver* driver = SceneManager->getVideoDriver();  
        mTexture = driver->getTexture(filename);                                       
		mMaterial.setTexture(0,mTexture);
/*       
		dimension2d<s32> size = (dimension2d<s32>)mTexture->getOriginalSize();
		/*fWidth  = (float)frameWidth/(float)size.Width;
		fHeight = (float)frameHeight/(float)size.Height;
		stepww = size.Width / frameWidth;
		stephh = size.Height / frameHeight;
		*/
                  
   }

	
};