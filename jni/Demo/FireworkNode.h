using namespace irr;
using namespace irr::core;
using namespace	irr::video;
using namespace	irr::scene;

#if defined(_IRR_ANDROID_PLATEFORM_)
#define FIRE_MAXVERTEX 1000
#define FIRE_MAXINDEX  1500
#else
#define FIRE_MAXVERTEX 1000
#define FIRE_MAXINDEX  1500
#endif

class FireworkNode : public Base2dNode
      {
         protected:
               
			   u32 mElapsedTime;
			   u32 mCreationTime;
			   s32 mLife;
			   f32 mFactor;

			   static u32 mVertexVBO;
			   static u32 mIndexVBO;
			   
			   
          public:

			  

			  FireworkNode(ISceneNode* parent, ISceneManager* mgr, s32 id,u32 creationTime,f32 factor): 
				Base2dNode(parent, mgr, id),mCreationTime(creationTime),mFactor(factor)
               {     
                      mMaterial.Wireframe = false;
                      mMaterial.Lighting = false;    					  
					  mMaterial.ZBuffer = ECFN_NEVER;
					  mMaterial.MaterialType = EMT_SOLID ;

					  mBox.reset(vector3df(-1.0,-1.0,-1.0));
					  mBox.addInternalPoint(vector3df(1.0,1.0,1.0));				   

					  mMaterial.setTexture(0,NULL); 
                      

               }     
            
			
			  s32 getLife()
			  {
				return mLife;

			  }

			  void setLife(u32 value)
			  {
				mLife = (s32)value;
				mElapsedTime=0;
			  }

			  void UpdateTime(u32 currentTime)
			  {

			    u32 deltaTime = currentTime - mCreationTime;
				mCreationTime = currentTime;
				mLife= mLife - deltaTime;
				mElapsedTime=mElapsedTime +deltaTime;
			  
			  }
			  
			  static void InitialzeVBO(float fWidth, float fHeight,ISceneManager* mgr);

			  virtual void render();

            
              
      };
