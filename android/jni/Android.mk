LOCAL_PATH := $(call my-dir)

##################################

include $(CLEAR_VARS)
LOCAL_MODULE := webp
LOCAL_SRC_FILES := webp/$(TARGET_ARCH_ABI)/libwebp.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/webp/include
LOCAL_EXPORT_CFLAGS := -DWITH_WEBP
include $(PREBUILT_STATIC_LIBRARY)

##################################

include $(CLEAR_VARS)
LOCAL_MODULE := tremolo
LOCAL_SRC_FILES := tremolo/$(TARGET_ARCH_ABI)/libtremolo.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/tremolo/include
LOCAL_EXPORT_CFLAGS := -DWITH_VORBIS
include $(PREBUILT_STATIC_LIBRARY)

##################################

include $(CLEAR_VARS)
LOCAL_MODULE := openal
LOCAL_SRC_FILES := openal/$(TARGET_ARCH_ABI)/libopenal.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/openal/include
include $(PREBUILT_SHARED_LIBRARY)

##################################

include $(CLEAR_VARS)

NCINE_ROOT=../..
INC=$(NCINE_ROOT)/include
SRC=$(NCINE_ROOT)/src
TST=$(NCINE_ROOT)/tests

LOCAL_MODULE := ncine
LOCAL_CFLAGS := -Werror -ffast-math
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(INC)
LOCAL_SRC_FILES := \
	main.cpp \
	$(SRC)/base/ncVector2f.cpp \
	$(SRC)/ncServiceLocator.cpp \
	$(SRC)/ncFileLogger.cpp \
	$(SRC)/ncTimer.cpp \
	$(SRC)/ncFrameTimer.cpp \
	$(SRC)/ncArrayIndexer.cpp \
	$(SRC)/ncProfileVariable.cpp \
	$(SRC)/ncApplication.cpp \
	$(SRC)/ncFont.cpp \
	$(SRC)/ncFontGlyph.cpp \
	$(SRC)/ncIFile.cpp \
	$(SRC)/ncStandardFile.cpp \
	$(SRC)/ncAssetFile.cpp \
	$(SRC)/ncAndroidInputManager.cpp \
	$(SRC)/graphics/ncIGfxDevice.cpp \
	$(SRC)/graphics/ncEGLGfxDevice.cpp \
	$(SRC)/graphics/ncGfxCapabilities.cpp \
	$(SRC)/graphics/ncTextureFormat.cpp \
	$(SRC)/graphics/ncITextureLoader.cpp \
	$(SRC)/graphics/ncTextureLoaderDDS.cpp \
	$(SRC)/graphics/ncTextureLoaderPVR.cpp \
	$(SRC)/graphics/ncTextureLoaderWebP.cpp \
	$(SRC)/graphics/ncTextureLoaderETC.cpp \
	$(SRC)/graphics/ncTexture.cpp \
	$(SRC)/graphics/ncProfilePlotter.cpp \
	$(SRC)/graphics/ncLinePlotter.cpp \
	$(SRC)/graphics/ncStackedBarPlotter.cpp \
	$(SRC)/graphics/ncSceneNode.cpp \
	$(SRC)/graphics/ncSprite.cpp \
	$(SRC)/graphics/ncRenderCommand.cpp \
	$(SRC)/graphics/ncRenderQueue.cpp \
	$(SRC)/graphics/ncSpriteBatchNode.cpp \
	$(SRC)/graphics/ncParticle.cpp \
	$(SRC)/graphics/ncParticleAffectors.cpp \
	$(SRC)/graphics/ncParticleSystem.cpp \
	$(SRC)/graphics/ncTextNode.cpp \
	$(SRC)/graphics/ncRectAnimation.cpp \
	$(SRC)/graphics/ncAnimatedSprite.cpp \
	$(SRC)/audio/ncALAudioDevice.cpp \
	$(SRC)/audio/ncIAudioLoader.cpp \
	$(SRC)/audio/ncAudioLoaderWav.cpp \
	$(SRC)/audio/ncAudioLoaderOgg.cpp \
	$(SRC)/audio/ncAudioBuffer.cpp \
	$(SRC)/audio/ncAudioStream.cpp \
	$(SRC)/audio/ncAudioBufferPlayer.cpp \
	$(SRC)/audio/ncAudioStreamPlayer.cpp \
	$(TST)/apptest_rotozoom.cpp

LOCAL_LDLIBS := -lm -llog -landroid -lEGL -lGLESv1_CM
LOCAL_STATIC_LIBRARIES := android_native_app_glue webp tremolo
LOCAL_SHARED_LIBRARIES := openal

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
