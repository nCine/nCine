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
LOCAL_CFLAGS := -Wall -ffast-math -DWITH_THREADS
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/$(INC) \
	$(LOCAL_PATH)/$(SRC)/include
LOCAL_SRC_FILES := \
	main.cpp \
	$(SRC)/base/Vector2f.cpp \
	$(SRC)/base/String.cpp \
	$(SRC)/ServiceLocator.cpp \
	$(SRC)/FileLogger.cpp \
	$(SRC)/Timer.cpp \
	$(SRC)/FrameTimer.cpp \
	$(SRC)/ArrayIndexer.cpp \
	$(SRC)/ProfileVariable.cpp \
	$(SRC)/Application.cpp \
	$(SRC)/Font.cpp \
	$(SRC)/FontGlyph.cpp \
	$(SRC)/IFile.cpp \
	$(SRC)/StandardFile.cpp \
	$(SRC)/android/AssetFile.cpp \
	$(SRC)/android/AndroidJniHelper.cpp \
	$(SRC)/android/AndroidInputManager.cpp \
	$(SRC)/graphics/IGfxDevice.cpp \
	$(SRC)/android/EglGfxDevice.cpp \
	$(SRC)/graphics/GfxCapabilities.cpp \
	$(SRC)/graphics/TextureFormat.cpp \
	$(SRC)/graphics/ITextureLoader.cpp \
	$(SRC)/graphics/TextureLoaderDds.cpp \
	$(SRC)/graphics/TextureLoaderPvr.cpp \
	$(SRC)/graphics/TextureLoaderWebP.cpp \
	$(SRC)/android/TextureLoaderEtc.cpp \
	$(SRC)/graphics/TextureLoaderKtx.cpp \
	$(SRC)/graphics/Texture.cpp \
	$(SRC)/graphics/ProfilePlotter.cpp \
	$(SRC)/graphics/PlottingVariable.cpp \
	$(SRC)/graphics/LinePlotter.cpp \
	$(SRC)/graphics/StackedBarPlotter.cpp \
	$(SRC)/graphics/DrawableNode.cpp \
	$(SRC)/graphics/SceneNode.cpp \
	$(SRC)/graphics/Sprite.cpp \
	$(SRC)/graphics/RenderCommand.cpp \
	$(SRC)/graphics/RenderQueue.cpp \
	$(SRC)/graphics/SpriteBatchNode.cpp \
	$(SRC)/graphics/Particle.cpp \
	$(SRC)/graphics/ParticleAffectors.cpp \
	$(SRC)/graphics/ParticleSystem.cpp \
	$(SRC)/graphics/TextNode.cpp \
	$(SRC)/graphics/RectAnimation.cpp \
	$(SRC)/graphics/AnimatedSprite.cpp \
	$(SRC)/audio/ALAudioDevice.cpp \
	$(SRC)/audio/IAudioLoader.cpp \
	$(SRC)/audio/AudioLoaderWav.cpp \
	$(SRC)/audio/AudioLoaderOgg.cpp \
	$(SRC)/audio/AudioBuffer.cpp \
	$(SRC)/audio/AudioStream.cpp \
	$(SRC)/audio/AudioBufferPlayer.cpp \
	$(SRC)/audio/AudioStreamPlayer.cpp \
	$(SRC)/threading/PosixThread.cpp \
	$(SRC)/threading/PosixThreadSync.cpp \
	$(SRC)/threading/ThreadPool.cpp \
	$(TST)/apptest_joystick.cpp

LOCAL_LDLIBS := -lm -llog -landroid -lEGL -lGLESv1_CM
LOCAL_STATIC_LIBRARIES := android_native_app_glue webp tremolo
LOCAL_SHARED_LIBRARIES := openal

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
