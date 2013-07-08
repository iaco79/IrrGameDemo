using namespace irr;
using namespace irr::core;
using namespace	irr::video;
using namespace	irr::scene;

class Base2dNode : public ISceneNode
      {
         protected:               
               
			   bool				   mIsTransparent;               			   
			   core::aabbox3d<f32> mBox;
			   video::SMaterial    mMaterial;
			   video::ITexture*    mTexture;
               
          public:
              Base2dNode(ISceneNode* parent, ISceneManager* mgr, s32 id): ISceneNode(parent, mgr, id)
               {     
				   irr::video::SColor scolor(255,255,255,255);	
                      mMaterial.Wireframe = false;
                      mMaterial.Lighting = false;
                      mIsTransparent = true;
					  mMaterial.AmbientColor = scolor;
					  mMaterial.DiffuseColor = scolor;
					  mMaterial.EmissiveColor  = scolor;

               }     
             
			  void setTransparent(bool value)
			  {
					mIsTransparent = value;
			  }

			  virtual void OnRegisterSceneNode()
			  {
				if (IsVisible)    
				{
					if(mIsTransparent)					
						SceneManager->registerNodeForRendering(this, ESNRP_TRANSPARENT);
					else
						SceneManager->registerNodeForRendering(this, ESNRP_SOLID);
				}

				ISceneNode::OnRegisterSceneNode();

			  }

              virtual void render() = 0;
              
              virtual const aabbox3d<f32>& getBoundingBox() const
               {
                  return mBox;
               }
     
			  virtual u32 getMaterialCount() const
              {
                  return 1;
              }
     		  
              virtual SMaterial& getMaterial(u32 i)
			  {
                 return  mMaterial;
              }
  
      };
