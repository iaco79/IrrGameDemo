#include <irrlicht.h>
#include "COGLES2Driver.h"
// needed here also because of the create methods' parameters
#include "CNullDriver.h"
#include "Base2dNode.h"
#include "FireworkNode.h"

extern void SetGlobalNodeElapsedTime(f32 value);


u32 FireworkNode::mVertexVBO=0;
u32 FireworkNode::mIndexVBO=0;


void FireworkNode::render()
{   
  IVideoDriver* driver = SceneManager->getVideoDriver();

	  
      driver->setMaterial(mMaterial);
	  IMaterialRenderer*mat = driver->getMaterialRenderer(mMaterial.MaterialType);

	  SetGlobalNodeElapsedTime((float)mElapsedTime * mFactor);

	  driver->setTransform(ETS_WORLD, AbsoluteTransformation);
    
	  COGLES2Driver* es2driver =  (COGLES2Driver*)driver;
	  es2driver->drawVertexPrimitiveListVBO(
			NULL,FIRE_MAXVERTEX,
			NULL, FIRE_MAXINDEX/3,
			EVT_STANDARD, 
			EPT_TRIANGLES,
			EIT_16BIT,
			mVertexVBO,
			mIndexVBO						
			);

				  
}
     

void FireworkNode::InitialzeVBO(float fWidth, float fHeight,ISceneManager* mgr)
{

	S3DVertex* Vertices = new S3DVertex[FIRE_MAXVERTEX];
	u16* Indices = new u16[FIRE_MAXINDEX];

	float radiusW = 0.05f;
	float radiusH = 0.6f;
	float angle = 720.0f /(FIRE_MAXVERTEX/4.0f);
	

	int q=0;

	for(int a=0;a<FIRE_MAXVERTEX;a+=4)
	{
		float step = (float)a/4.0;
		int  nn = a/4;

		float nangle = (step*angle +   0.0f)*3.1416f/180.0f;
		float vangle = (step*angle +  90.0f)*3.1416f/180.0f;
		float vangle2 =(step*angle + 270.0f)*3.1416f/180.0f;

		//float dangle = (angle/2)*3.1416/180;
		float nx=cos(nangle); 
		float ny=sin(nangle); 

		float speedi= ((float)(rand()%30)/100.0f)+0.05f;		
		float speedo= speedi; //((float)(rand()%3+1)/100.0f) + speedi;//0.03f;

		if(nn> FIRE_MAXVERTEX/4/2)
		{
			if(nn%2)
				speedi= ((float)(rand()%5 + 20.0f)/100.0f)+0.05f;					
			else
				speedi= ((float)(rand()%8 + 10.0f)/100.0f)+0.05f;					
			
			speedo= speedi; //((float)(rand()%2+1)/100.0f) + speedi;//0.03f;
		}

		SColor extracolor(255,255,255,64);

		int j = rand()%4;

		switch(j)
		{
			case 0:extracolor=SColor(255,255,255,0);break;
			case 1:extracolor=SColor(255,255,128,0);break;
			case 2:extracolor=SColor(255,255,0,90);break;
			case 3:extracolor=SColor(255,255,0,200);break;			
		}



		

		//SColor extracolor(255,200,0,255);

		
		float x = cos(vangle)*radiusW + nx*radiusH; 
		float y = sin(vangle)*radiusW + ny*radiusH; 	
		extracolor.setAlpha(255);
		Vertices[a+0] = S3DVertex(x,y,0, nx*speedo,ny*speedo,1,extracolor,0,1);

		x = cos(vangle2)*radiusW + nx*radiusH; 
		y = sin(vangle2)*radiusW + ny*radiusH; 
		Vertices[a+1] = S3DVertex(x,y,0, nx*speedo,ny*speedo,1,extracolor,0,1);	


		extracolor.setAlpha(extracolor.getAlpha()/2);
		/*switch(j)
		{
			case 0:extracolor=SColor(255,255,32,200);break;
			case 1:extracolor=SColor(255,255,0,0);break;
			case 2:extracolor=SColor(255,255,100,0);break;
		}*/		


		extracolor.setAlpha(100);
		x = cos(vangle)*radiusW; 
		y = sin(vangle)*radiusW; 
		Vertices[a+2] = S3DVertex(x,y,0, nx*speedi,ny*speedi,0,extracolor,0,1);	

		x = cos(vangle2)*radiusW; 
		y = sin(vangle2)*radiusW; 
		Vertices[a+3] = S3DVertex(x,y,0, nx*speedi,ny*speedi,0,extracolor,0,1);	

	   float tx = 0;
	   float ty = 0;
	   Vertices[a+0].TCoords.X = tx;
	   Vertices[a+0].TCoords.Y = ty;
	   Vertices[a+1].TCoords.X = tx+1.0f;
	   Vertices[a+1].TCoords.Y = ty;
	   Vertices[a+2].TCoords.X = tx;
	   Vertices[a+2].TCoords.Y = ty+1.0f;
	   Vertices[a+3].TCoords.X = tx+1.0f;
	   Vertices[a+3].TCoords.Y = ty+1.0f;
       	               
	   /*Indices[q+0] = a+1;
	   Indices[q+1] = a+0;
	   Indices[q+2] = a+2;

	   Indices[q+3] = a+2;
	   Indices[q+4] = a+3;
	   Indices[q+5] = a+1;
	   */
	   Indices[q+0] = a+0;
	   Indices[q+1] = a+1;
	   Indices[q+2] = a+3;

	   Indices[q+3] = a+0;
	   Indices[q+4] = a+3;
	   Indices[q+5] = a+2;

	   q+=6;
	
	}


  IVideoDriver* driver = mgr->getVideoDriver();
  COGLES2Driver* es2driver =  (COGLES2Driver*)driver;

  es2driver->genVBO(Vertices,FIRE_MAXVERTEX,
					Indices,FIRE_MAXINDEX,
					mVertexVBO,
					mIndexVBO);

  delete[] Vertices;
  delete[] Indices;
}


