//Our firework material

class ClearMaterialCallBack : public video::IShaderConstantSetCallBack
{
public:

	IrrlichtDevice* mDevice;

	static s32 gClearMaterialType;

	ClearMaterialCallBack(IrrlichtDevice* dev) :mDevice(dev){;}

	static void Initialize(IrrlichtDevice* dev)
	  {
		  video::IGPUProgrammingServices* gpu = dev->getVideoDriver()->getGPUProgrammingServices();
	
		  ClearMaterialCallBack* mc = new ClearMaterialCallBack(dev);

		  gClearMaterialType = gpu->addHighLevelShaderMaterialFromFiles(
		#if defined(_IRR_ANDROID_PLATEFORM_)
  			"/mnt/sdcard/GameDemo/ClearShader.vsh", "main", video::EVST_VS_1_1,
			"/mnt/sdcard/GameDemo/ClearShader.fsh", "main", video::EPST_PS_1_1,
		#else
	  	  "IrrlichtSdcard/ClearShader.vsh", "main", video::EVST_VS_1_1,
		  "IrrlichtSdcard/ClearShader.fsh", "main", video::EPST_PS_1_1,
		#endif			
			mc, 
			video::EMT_TRANSPARENT_ALPHA_CHANNEL,
			NULL);

		  mc->drop();

	  }

	virtual void OnSetMaterial(const SMaterial& material) 
	{ 
		COGLES2Driver* es2driver =  (COGLES2Driver*)mDevice->getVideoDriver();		
		es2driver->blendFunc( EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA);
		es2driver->enableBlend();
		
	}

	virtual void OnSetConstants(video::IMaterialRendererServices* services,
			s32 userData)
	{		
		
		
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
	
		
	}
};
