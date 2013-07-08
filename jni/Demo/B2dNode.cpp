#include <irrlicht.h>
#include <Box2D/Box2D.h>
#include "Base2dNode.h"
#include "B2dNode.h"
#include <new>

using namespace irr;

extern bool useWhite;

void B2dNode::CreateGeometryFromFixture(b2Fixture* fixture,int fixtureIndex,video::SColor scolor)
{
	int type = fixture->GetType();

	if(type == b2Shape::e_polygon)
	{
		b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
		mVertexCount[fixtureIndex] =  poly->m_vertexCount;
		mShapeType[fixtureIndex]=0; //polygon

		video::S3DVertex* irrVertex= new video::S3DVertex[mVertexCount[fixtureIndex]];
		u16* irrIndex = new u16[mVertexCount[fixtureIndex]];
		
		for (int32 i= 0; i < mVertexCount[fixtureIndex]; i++)
		{
			irrVertex[i].Pos = core::vector3df(poly->m_vertices[i].x,poly->m_vertices[i].y,0);		
			
			irrVertex[i].Normal = irrVertex[i].Pos;
			irrVertex[i].Normal = irrVertex[i].Normal.normalize();

			irrVertex[i].Color = scolor;
			irrIndex[i]=mVertexCount[fixtureIndex]-i-1;
		}		
	

		mVertices[fixtureIndex]= irrVertex;
		mIndices[fixtureIndex] = irrIndex;
	}
	else if(type == b2Shape::e_edge)
	{
		mShapeType[fixtureIndex]=1; //segment
		mVertexCount[fixtureIndex]=2;

		video::S3DVertex* irrVertex= new video::S3DVertex[mVertexCount[fixtureIndex]];
		u16* irrIndex = new u16[mVertexCount[fixtureIndex]];
	
		b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();


		b2Vec2 v1 = edge->m_vertex1;
		b2Vec2 v2 = edge->m_vertex2;

		irrVertex[0].Pos = core::vector3df(v1.x,v1.y,0);		
		irrIndex[0]=0;
		irrVertex[0].Color = scolor;

		irrVertex[1].Pos = core::vector3df(v2.x,v2.y,0);		
		irrIndex[1]=1;
		irrVertex[1].Color = scolor;
	
		mVertices[fixtureIndex]= irrVertex;
		mIndices[fixtureIndex]= irrIndex;
	}

	

}


void B2dNode::CreateGeometryFromBody(b2Body *body,video::SColor scolor)
{	
	mFixtureCount=0;
	
	for(b2Fixture* fixs = body->GetFixtureList(); fixs; fixs=fixs->GetNext() )
	{
		if(mFixtureCount<MAXFIXTURECOUNT)
		{
			CreateGeometryFromFixture(fixs,mFixtureCount,scolor);	
			mFixtureCount++;		
		}
	}	
}

B2dNode::B2dNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id,b2Body* body,video::SColor scolor) :
		Base2dNode(parent, mgr, id)
{
	mMaterial.Wireframe = false;
	mMaterial.Lighting = false;
	mMaterial.AmbientColor = scolor;
	mMaterial.DiffuseColor = scolor;
	mMaterial.EmissiveColor  = scolor;	
	mMaterial.Thickness=3.0f;
		

	//Create Vertices and indices from body def
	CreateGeometryFromBody(body,scolor);

	//Set Bounding Box
	for(int j=0;j<mFixtureCount;j++)
	{
		video::S3DVertex* irrVertex = mVertices[j];
		int vertexCount = mVertexCount[j];

		for(int i=0;i<vertexCount;i++)
		{
			if(i==0 && j==0)			
				mBox.reset(irrVertex[i].Pos);			
			else
				mBox.addInternalPoint(irrVertex[i].Pos);
		}
	}	
}


void B2dNode::render()
{

	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	driver->setMaterial(mMaterial);
	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	for(int j=0;j<mFixtureCount;j++)
	{
		int shapeType = mShapeType[j];
		video::S3DVertex* irrVertex = mVertices[j];
		u16* irrIndex = mIndices[j];
		int vertexCount = mVertexCount[j];

		if(shapeType==0)
		{
			
			video::SColor lineColor = irrVertex[0].Color;
			lineColor.setAlpha(128);
			
			if(useWhite)
			{
				lineColor = video::SColor(255,255,255,255);

				for(int i=1;i<vertexCount;i++)
				{
					driver->draw3DLine(irrVertex[i-1].Pos,irrVertex[i].Pos,lineColor);
				}
				driver->draw3DLine(irrVertex[vertexCount	-1].Pos,irrVertex[0].Pos,lineColor);
			

			}			
			else 
			{
				driver->drawIndexedTriangleFan(irrVertex,vertexCount, irrIndex,vertexCount-2);
			}
									
			
			
		}
		else
		{
			driver->draw3DLine(irrVertex[0].Pos,irrVertex[1].Pos,irrVertex[0].Color);
			
		}
	}		

}

