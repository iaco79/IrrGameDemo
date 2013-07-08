//Our firework material

typedef f32 (*GetIntFunctor)(void);

class FireWorkShaderCallBack : public video::IShaderConstantSetCallBack
{
public:

	IrrlichtDevice* mDevice;

	static s32 gFireWorkMaterialType;

	FireWorkShaderCallBack(IrrlichtDevice* dev) :mDevice(dev){;}

	static void Initialize(IrrlichtDevice* dev, GetIntFunctor f )
	  {
		  video::IGPUProgrammingServices* gpu = dev->getVideoDriver()->getGPUProgrammingServices();
	
		  FireWorkShaderCallBack* mc = new FireWorkShaderCallBack(dev);

		  gFireWorkMaterialType = gpu->addHighLevelShaderMaterialFromFiles(
		#if defined(_IRR_ANDROID_PLATEFORM_)
  			"/mnt/sdcard/GameDemo/MyShader.vsh", "main", video::EVST_VS_1_1,
			"/mnt/sdcard/GameDemo/MyShader.fsh", "main", video::EPST_PS_1_1,
		#else
	  	  "IrrlichtSdcard/MyShader.vsh", "main", video::EVST_VS_1_1,
		  "IrrlichtSdcard/MyShader.fsh", "main", video::EPST_PS_1_1,
		#endif			
			mc, 
			video::EMT_TRANSPARENT_ALPHA_CHANNEL,
			reinterpret_cast<s32>(f));

		  mc->drop();

	  }

	virtual void OnSetMaterial(const SMaterial& material) 
	{ 
		COGLES2Driver* es2driver =  (COGLES2Driver*)mDevice->getVideoDriver();
		es2driver->blendFunc( EBF_SRC_ALPHA, EBF_ONE);
		

	}

	virtual void OnSetConstants(video::IMaterialRendererServices* services,
			s32 userData)
	{		
		
		
		GetIntFunctor fn = (GetIntFunctor)reinterpret_cast<void*>(userData);


		f32 timer = fn();
		f32 elapsedTime  = timer;


		video::IVideoDriver* driver = services->getVideoDriver();
		core::matrix4 worldViewProj;
		worldViewProj = driver->getTransform(video::ETS_PROJECTION);
		worldViewProj *= driver->getTransform(video::ETS_VIEW);
		worldViewProj *= driver->getTransform(video::ETS_WORLD);
		services->setVertexShaderConstant("uMvpMatrix", worldViewProj.pointer(), 1);
	
		
		core::vector3df pos = mDevice->getSceneManager()->getActiveCamera()->getAbsolutePosition();		
		core::matrix4 world = driver->getTransform(video::ETS_WORLD);
		world = world.getTransposed();
		services->setVertexShaderConstant("uWorldMatrix", world.pointer(), 1);

		services->setVertexShaderConstant("uTimer", &elapsedTime, 1);
		
		
	}
};
