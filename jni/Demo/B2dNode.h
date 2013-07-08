

using namespace irr;

#define MAXFIXTURECOUNT 4

class B2dNode : public Base2dNode
{

private:

	video::S3DVertex* mVertices[MAXFIXTURECOUNT];
	u16* mIndices[MAXFIXTURECOUNT];	
	int mVertexCount[MAXFIXTURECOUNT];
	int mShapeType[MAXFIXTURECOUNT];
	int mFixtureCount;
	
	
	~B2dNode() // destructor
    {
		for(int i=0;i<mFixtureCount;i++)
		{
			delete[] mVertices[i];
			delete[] mIndices[i];
		}
 
    };

	void CreateGeometryFromFixture(b2Fixture* fixture,int fixtureIndex,video::SColor scolor);
	void CreateGeometryFromBody(b2Body* body,video::SColor scolor);

public:

	B2dNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id,b2Body* body, video::SColor scolor);
	
	virtual void render();

	

};