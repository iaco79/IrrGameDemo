#####
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libogg_static

LOCAL_ARM_MODE   := arm 

LOCAL_SRC_FILES := src/bitwise.c \
	src/framing.c 
		
	
LOCAL_CFLAGS := -DANDROID_NDK  -DFT2_BUILD_LIBRARY=1  -I./jni/libogg-1.3.0/include

LOCAL_LDLIBS := -ldl -llog

include $(BUILD_STATIC_LIBRARY)
#####