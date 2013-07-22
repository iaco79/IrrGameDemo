//Node to draw one sprite

using namespace irr;
using namespace irr::core;
using namespace	irr::video;
using namespace	irr::scene;

class SpriteNode : public Base2dNode
      {
         protected:
               
               video::S3DVertex    Vertices[4];
               u16                 Indices[6];
               f32                 fWidth,fHeight;               
               s32                 stepww,stephh;               
          public:
              SpriteNode(ISceneNode* parent, ISceneManager* mgr, s32 id): Base2dNode(parent, mgr, id) 				
               {     
					u16 ind[] = {1,0,2, 2,0,3}; 
                    memcpy(Indices,ind,sizeof(u16)*6);
               }     
             
				virtual void Load(char* filename,s32 
					frameWidth,
					s32 frameHeight, 
					float width, 
					float height)
               {
				   float x = width/2.0f;
                   float y = height/2.0f;
                   IVideoDriver* driver = SceneManager->getVideoDriver();
                   
                   Vertices[0] = S3DVertex(-x,-y,0, 0,0,0,SColor(255,255,255,255),0,1);
                   Vertices[1] = S3DVertex( x,-y,0, 0,0,0,SColor(255,255,255,255),1,1);
                   Vertices[2] = S3DVertex( x, y,0, 0,0,0,SColor(255,255,255,255),1,0);
                   Vertices[3] = S3DVertex(-x, y,0, 0,0,0,SColor(255,255,255,255),0,0);                   

				   //set bounding box
                   mBox.reset(Vertices[0].Pos);
                   for (s32 i=1; i<4; ++i)  
					   mBox.addInternalPoint(Vertices[i].Pos);
                   
				   //set material texture
                   mTexture = driver->getTexture(filename);
                   
				   mMaterial.setTexture(0,mTexture);
                   mMaterial.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;
				 
                   
				   //get frame width and weight in uv coordinates
                   dimension2d<s32> size = (dimension2d<s32>)mTexture->getOriginalSize();
                   fWidth  = (float)frameWidth/(float)size.Width;
                   fHeight = (float)frameHeight/(float)size.Height;                   
                   stepww = size.Width / frameWidth;
                   stephh = size.Height / frameHeight;
                                 
                   setFrame(0);

               }
               
			  void SetVertexColor(video::SColor& color)
			  {	                              
				 Vertices[0].Color = color;
				 Vertices[1].Color = color;
				 Vertices[2].Color = color;
				 Vertices[3].Color = color;
			  }

              virtual void setFrame(int n)
               {
                   float x = (n % stepww)*fWidth;
                   float y = (n / stepww)*fHeight;
                   Vertices[0].TCoords.X = x;
                   Vertices[0].TCoords.Y = y+fHeight;
                   Vertices[1].TCoords.X = x+fWidth;
                   Vertices[1].TCoords.Y = y+fHeight;
                   Vertices[2].TCoords.X = x+fWidth;
                   Vertices[2].TCoords.Y = y;
                   Vertices[3].TCoords.X = x;
                   Vertices[3].TCoords.Y = y;
                   
               }
              
              virtual void render()
               {                    
                  IVideoDriver* driver = SceneManager->getVideoDriver();
                  driver->setMaterial(mMaterial);				  

                  driver->setTransform(ETS_WORLD, AbsoluteTransformation);                  
				  driver->drawVertexPrimitiveList(&Vertices[0],4,&Indices[0],2);
               }

			   virtual void LoadFromTexture(ITexture* texture,s32 
				   frameWidth,s32 
				   frameHeight, 
				   float width, float 
				   height)
			  {
				  IVideoDriver* driver = SceneManager->getVideoDriver();
                   float x = width/2.0f;
                   float y = height/2.0f;
                   Vertices[0] = S3DVertex(-x,-y,0, 0,0,0,SColor(255,255,255,255),0,1);
                   Vertices[1] = S3DVertex( x,-y,0, 0,0,0,SColor(255,255,255,255),1,1);
                   Vertices[2] = S3DVertex( x, y,0, 0,0,0,SColor(255,255,255,255),1,0);
                   Vertices[3] = S3DVertex(-x, y,0, 0,0,0,SColor(255,255,255,255),0,0);
                
                   //set bounding box
                   mBox.reset(Vertices[0].Pos);
                   for (s32 i=1; i<4; ++i)  
					   mBox.addInternalPoint(Vertices[i].Pos);
                   
                   mTexture = texture;
                   
				   mMaterial.setTexture(0,mTexture);
                   mMaterial.MaterialType = EMT_SOLID;
				   
                   dimension2d<s32> size = (dimension2d<s32>)mTexture->getOriginalSize();
                   fWidth  = (float)frameWidth/(float)size.Width;
                   fHeight = (float)frameHeight/(float)size.Height;
                   
                   stepww = size.Width / frameWidth;
                   stephh = size.Height / frameHeight;
                                 
                   Vertices[0].TCoords.X = 0;
                   Vertices[0].TCoords.Y = 0;

                   Vertices[1].TCoords.X = 1;
                   Vertices[1].TCoords.Y = 0;

                   Vertices[2].TCoords.X = 1;
                   Vertices[2].TCoords.Y = 1;

                   Vertices[3].TCoords.X = 0;
                   Vertices[3].TCoords.Y = 1;
			  
			  }

     		  
             
      };
