#####
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libtremor_static


LOCAL_ARM_MODE   := arm 

LOCAL_SRC_FILES := \
	block.c \
	codebook.c \
	floor0.c \
	floor1.c \
	info.c \
	mapping0.c \
	mdct.c \
	registry.c \
	res012.c \
	sharedbook.c \
	synthesis.c \
	vorbisfile.c \
	window.c
		
	
LOCAL_CFLAGS := -DANDROID_NDK -D_ARM_ASSEM_ -DFT2_BUILD_LIBRARY=1 -D_LIB -I./jni/libogg-1.3.0/include  -I./jni/tremor

LOCAL_LDLIBS := -ldl -llog

include $(BUILD_STATIC_LIBRARY)
#####