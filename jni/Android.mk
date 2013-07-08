# 
# COSOperator.cpp was removed from IRROTHEROBJ
LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


DEMOSRC =  Demo/B2dNode.cpp Demo/Box2dTest.cpp Demo/GameObject.cpp Demo/SpriteNode.cpp Demo/Player.cpp Demo/FireworkNode.cpp Demo/BatchedSpriteNode.cpp Demo/Base2dNode.cpp
DEMOSRCROCKET= Demo/RocketInterface/CIrrRocketEventReceiver.cpp Demo/RocketInterface/CIrrRocketGUI.cpp Demo/RocketInterface/IrrRocketFileInterface.cpp Demo/RocketInterface/IrrRocketInterface.cpp Demo/RocketInterface/IrrRocketRenderer.cpp
DEMOAUDIO= Demo/Audio/AudioManager.cpp
ANDROID = app-android.cpp android-activity.cpp android-receiver.cpp


LOCAL_MODULE := gamedemo
LOCAL_ARM_MODE   := arm 
LOCAL_CFLAGS := -O3 -DANDROID_NDK -DDISABLE_IMPORTGL -I./../include/  -I./jni/irrlicht/ -I./jni/irrlicht/include/ -I./jni/Demo/  -I./jni/freetype2_static/include  -I./jni/freetype2_static/src -I./jni/librocket/include -I./jni/SDL/include -I./jni/SDL_Mixer
																							      

LOCAL_SRC_FILES := \
 	$(DEMOSRC) \
 	$(DEMOSRCROCKET) \
 	$(DEMOAUDIO) \
    $(ANDROID)


LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lGLESv2  
LOCAL_STATIC_LIBRARIES := \
                librocket_static \
				libfreetype2_static \
				libsdl2_mixer_static \
                libsdl2_static \
                libtremor_static \
				libogg_static \
				irrlicht_static \
				box2d_static				

include $(BUILD_SHARED_LIBRARY)


$(call import-module,box2d)
$(call import-module,irrlicht)
$(call import-module,freetype2_static)
$(call import-module,librocket)
$(call import-module,libogg-1.3.0)
$(call import-module,tremor)
$(call import-module,sdl)
$(call import-module,sdl_mixer)

