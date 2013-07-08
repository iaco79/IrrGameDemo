// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSkyBoxSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"
#include "S3DVertex.h"
#include "os.h"

namespace irr
{
namespace scene
{

//! constructor
CSkyBoxSceneNode::CSkyBoxSceneNode(video::ITexture* top, video::ITexture* bottom, video::ITexture* left,
			video::ITexture* right, video::ITexture* front, video::ITexture* back, ISceneNode* parent, ISceneManager* mgr, s32 id)
: ISceneNode(parent, mgr, id)
{
	#ifdef _DEBUG
	setDebugName("CSkyBoxSceneNode");
	#endif

	setAutomaticCulling(scene::EAC_OFF);
	Box.MaxEdge.set(0,0,0);
	Box.MinEdge.set(0,0,0);

	// create indices

	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;
	Indices[3] = 3;

	// create material

	video::SMaterial mat;
	mat.Lighting = false;
	mat.ZBuffer = video::ECFN_NEVER;
	mat.ZWriteEnable = false;
	mat.AntiAliasing=0;
	//mat.TextureLayer[0].TextureWrapU = video::ETC_CLAMP_TO_EDGE;
	//mat.TextureLayer[0].TextureWrapV = video::ETC_CLAMP_TO_EDGE;

	/* Hey, I am no artist, but look at that
	   cool ASCII art I made! ;)

       -111         111
          /6--------/5        y
         /  |      / |        ^  z
        /   |   11-1 |        | /
  -11-1 3---------2  |        |/
        |   7- - -| -4 1-11    *---->x
        | -1-11   |  /       3-------|2
        |/        | /         |    //|
        0---------1/          |  //  |
     -1-1-1     1-1-1         |//    |
	                     0--------1
	*/

	video::ITexture* tex = front;
	/*if (!tex) tex = left;
	if (!tex) tex = back;
	if (!tex) tex = right;
	if (!tex) tex = top;
	if (!tex) tex = bottom;
	*/

	const f32 onepixel = 0.0f; //tex?(1.0f / (tex->getSize().Width * 1.5f)) : 0.0f;
	const f32 t = 1.0f - onepixel;
	const f32 o = 0.0f + onepixel;

	// create front side

	//IACO begin
	tex = top;
	mat.setTexture(0,top);
	//IACO end
	
	Material[0] = mat;
	//Material[0].setTexture(0, front);
	Material[0].setTexture(0, top);
	Vertices[0] = video::S3DVertex(-1,-1,-1, 0,0,1, video::SColor(255,255,255,255), t, t);
	Vertices[1] = video::S3DVertex( 1,-1,-1, 0,0,1, video::SColor(255,255,255,255), o, t);
	Vertices[2] = video::S3DVertex( 1, 1,-1, 0,0,1, video::SColor(255,255,255,255), o, o);
	Vertices[3] = video::S3DVertex(-1, 1,-1, 0,0,1, video::SColor(255,255,255,255), t, o);

	//IACO begin
	
	irr::core::dimension2d<s32> size = (irr::core::dimension2d<s32>)tex->getOriginalSize();

	float fWidth  = 128.0f/(float)size.Width;
	float fHeight = 128.0f/(float)size.Height;

	s32 stepww = size.Width/128.0f;	

	float x = (5 % stepww)*fWidth;
    float y = (5 / stepww)*fHeight;
	Vertices[0].TCoords.X = x;
	Vertices[0].TCoords.Y = y+fHeight;
	Vertices[1].TCoords.X = x+fWidth;
	Vertices[1].TCoords.Y = y+fHeight;
	Vertices[2].TCoords.X = x+fWidth;
	Vertices[2].TCoords.Y = y;
	Vertices[3].TCoords.X = x;
	Vertices[3].TCoords.Y = y;
	//IACO end


	// create left side

	Material[1] = mat;
	//Material[1].setTexture(0, left);
	Material[1].setTexture(0, top);
	Vertices[4] = video::S3DVertex( 1,-1,-1, -1,0,0, video::SColor(255,255,255,255), t, t);
	Vertices[5] = video::S3DVertex( 1,-1, 1, -1,0,0, video::SColor(255,255,255,255), o, t);
	Vertices[6] = video::S3DVertex( 1, 1, 1, -1,0,0, video::SColor(255,255,255,255), o, o);
	Vertices[7] = video::S3DVertex( 1, 1,-1, -1,0,0, video::SColor(255,255,255,255), t, o);


	//IACO begin
	x = (6 % stepww)*fWidth;
    y = (6 / stepww)*fHeight;
	Vertices[4].TCoords.X = x;
	Vertices[4].TCoords.Y = y+fHeight;
	Vertices[5].TCoords.X = x+fWidth;
	Vertices[5].TCoords.Y = y+fHeight;
	Vertices[6].TCoords.X = x+fWidth;
	Vertices[6].TCoords.Y = y;
	Vertices[7].TCoords.X = x;
	Vertices[7].TCoords.Y = y;
	//IACO end


	// create back side

	Material[2] = mat;
	//Material[2].setTexture(0, back);
	Material[2].setTexture(0, top);
	Vertices[8]  = video::S3DVertex( 1,-1, 1, 0,0,-1, video::SColor(255,255,255,255), t, t);
	Vertices[9]  = video::S3DVertex(-1,-1, 1, 0,0,-1, video::SColor(255,255,255,255), o, t);
	Vertices[10] = video::S3DVertex(-1, 1, 1, 0,0,-1, video::SColor(255,255,255,255), o, o);
	Vertices[11] = video::S3DVertex( 1, 1, 1, 0,0,-1, video::SColor(255,255,255,255), t, o);


	//IACO begin
	x = (7 % stepww)*fWidth;
    y = (7 / stepww)*fHeight;
	Vertices[8].TCoords.X = x;
	Vertices[8].TCoords.Y = y+fHeight;
	Vertices[9].TCoords.X = x+fWidth;
	Vertices[9].TCoords.Y = y+fHeight;
	Vertices[10].TCoords.X = x+fWidth;
	Vertices[10].TCoords.Y = y;
	Vertices[11].TCoords.X = x;
	Vertices[11].TCoords.Y = y;
	//IACO end


	// create right side

	Material[3] = mat;
	//Material[3].setTexture(0, right);
	Material[3].setTexture(0, top);
	Vertices[12] = video::S3DVertex(-1,-1, 1, 1,0,0, video::SColor(255,255,255,255), t, t);
	Vertices[13] = video::S3DVertex(-1,-1,-1, 1,0,0, video::SColor(255,255,255,255), o, t);
	Vertices[14] = video::S3DVertex(-1, 1,-1, 1,0,0, video::SColor(255,255,255,255), o, o);
	Vertices[15] = video::S3DVertex(-1, 1, 1, 1,0,0, video::SColor(255,255,255,255), t, o);


	//IACO begin
	x = (4 % stepww)*fWidth;
    y = (4 / stepww)*fHeight;
	Vertices[12].TCoords.X = x;
	Vertices[12].TCoords.Y = y+fHeight;
	Vertices[13].TCoords.X = x+fWidth;
	Vertices[13].TCoords.Y = y+fHeight;
	Vertices[14].TCoords.X = x+fWidth;
	Vertices[14].TCoords.Y = y;
	Vertices[15].TCoords.X = x;
	Vertices[15].TCoords.Y = y;
	//IACO end



	// create top side

	Material[4] = mat;
	//Material[4].setTexture(0, top);
	Material[4].setTexture(0, top);
	Vertices[16] = video::S3DVertex( 1, 1,-1, 0,-1,0, video::SColor(255,255,255,255), t, t);
	Vertices[17] = video::S3DVertex( 1, 1, 1, 0,-1,0, video::SColor(255,255,255,255), o, t);
	Vertices[18] = video::S3DVertex(-1, 1, 1, 0,-1,0, video::SColor(255,255,255,255), o, o);
	Vertices[19] = video::S3DVertex(-1, 1,-1, 0,-1,0, video::SColor(255,255,255,255), t, o);

	//IACO begin
	x = (9 % stepww)*fWidth;
    y = (9 / stepww)*fHeight;
	Vertices[16].TCoords.X = x;
	Vertices[16].TCoords.Y = y+fHeight;
	Vertices[17].TCoords.X = x+fWidth;
	Vertices[17].TCoords.Y = y+fHeight;
	Vertices[18].TCoords.X = x+fWidth;
	Vertices[18].TCoords.Y = y;
	Vertices[19].TCoords.X = x;
	Vertices[19].TCoords.Y = y;
	//IACO end



	// create bottom side

	Material[5] = mat;
	//Material[5].setTexture(0, bottom);
	Material[5].setTexture(0, top);
	Vertices[20] = video::S3DVertex( 1,-1, 1, 0,1,0, video::SColor(255,255,255,255), o, o);
	Vertices[21] = video::S3DVertex( 1,-1,-1, 0,1,0, video::SColor(255,255,255,255), t, o);
	Vertices[22] = video::S3DVertex(-1,-1,-1, 0,1,0, video::SColor(255,255,255,255), t, t);
	Vertices[23] = video::S3DVertex(-1,-1, 1, 0,1,0, video::SColor(255,255,255,255), o, t);

	//IACO begin
	x = (1 % stepww)*fWidth;
    y = (1 / stepww)*fHeight;
	Vertices[20].TCoords.X = x;
	Vertices[20].TCoords.Y = y+fHeight;
	Vertices[21].TCoords.X = x+fWidth;
	Vertices[21].TCoords.Y = y+fHeight;
	Vertices[22].TCoords.X = x+fWidth;
	Vertices[22].TCoords.Y = y;
	Vertices[23].TCoords.X = x;
	Vertices[23].TCoords.Y = y;
	//IACO end


}


//! renders the node.
void CSkyBoxSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();

	if (!camera || !driver)
		return;

	if ( !camera->isOrthogonal() )
	{
		// draw perspective skybox

		core::matrix4 translate(AbsoluteTransformation);
		translate.setTranslation(camera->getAbsolutePosition());

		// Draw the sky box between the near and far clip plane
		const f32 viewDistance = (camera->getNearValue() + camera->getFarValue()) * 0.5f;
		core::matrix4 scale;
		scale.setScale(core::vector3df(viewDistance, viewDistance, viewDistance));

		driver->setTransform(video::ETS_WORLD, translate * scale);


		driver->setMaterial(Material[0]);

		for (s32 i=0; i<4; ++i)
		{
			
			driver->drawIndexedTriangleFan(&Vertices[i*4], 4, Indices, 2);
		}
	}
	else
	{
		// draw orthogonal skybox,
		// simply choose one texture and draw it as 2d picture.
		// there could be better ways to do this, but currently I think this is ok.

		core::vector3df lookVect = camera->getTarget() - camera->getAbsolutePosition();
		lookVect.normalize();
		core::vector3df absVect( core::abs_(lookVect.X),
					 core::abs_(lookVect.Y),
					 core::abs_(lookVect.Z));

		int idx = 0;

		if ( absVect.X >= absVect.Y && absVect.X >= absVect.Z )
		{
			// x direction
			idx = lookVect.X > 0 ? 0 : 2;
		}
		else
		if ( absVect.Y >= absVect.X && absVect.Y >= absVect.Z )
		{
			// y direction
			idx = lookVect.Y > 0 ? 4 : 5;
		}
		else
		if ( absVect.Z >= absVect.X && absVect.Z >= absVect.Y )
		{
			// z direction
			idx = lookVect.Z > 0 ? 1 : 3;
		}

		video::ITexture* tex = Material[idx].getTexture(0);

		if ( tex )
		{
			core::rect<s32> rctDest(core::position2d<s32>(-1,0),
									core::dimension2di(driver->getCurrentRenderTargetSize()));
			core::rect<s32> rctSrc(core::position2d<s32>(0,0),
									core::dimension2di(tex->getSize()));

			driver->draw2DImage(tex, rctDest, rctSrc);
		}
	}
}



//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CSkyBoxSceneNode::getBoundingBox() const
{
	return Box;
}


void CSkyBoxSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this, ESNRP_SKY_BOX);

	ISceneNode::OnRegisterSceneNode();
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CSkyBoxSceneNode::getMaterial(u32 i)
{
	return Material[i];
}


//! returns amount of materials used by this scene node.
u32 CSkyBoxSceneNode::getMaterialCount() const
{
	return 6;
}


//! Creates a clone of this scene node and its children.
ISceneNode* CSkyBoxSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
{
	if (!newParent) newParent = Parent;
	if (!newManager) newManager = SceneManager;

	CSkyBoxSceneNode* nb = new CSkyBoxSceneNode(0,0,0,0,0,0, newParent,
		newManager, ID);

	nb->cloneMembers(this, newManager);

	for (u32 i=0; i<6; ++i)
		nb->Material[i] = Material[i];

	nb->drop();
	return nb;
}


} // end namespace scene
} // end namespace irr

