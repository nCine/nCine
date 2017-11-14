#include "common_macros.h"
#include "AndroidJniHelper.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

JavaVM *AndroidJniHelper::javaVM_ = NULL;
JNIEnv *AndroidJniHelper::jniEnv_ = NULL;
JNIEnv *AndroidJniClass::jniEnv_ = NULL;
unsigned int AndroidJniHelper::sdkVersion_ = 0;

jclass AndroidJniClass_Version::javaClass_ = NULL;
jfieldID AndroidJniClass_Version::fidSdkInt_ = NULL;
jclass AndroidJniClass_InputDevice::javaClass_ = NULL;
jmethodID AndroidJniClass_InputDevice::midGetDevice_ = NULL;
jmethodID AndroidJniClass_InputDevice::midGetDeviceIds_ = NULL;
jmethodID AndroidJniClass_InputDevice::midGetName_ = NULL;
jmethodID AndroidJniClass_InputDevice::midGetProductId_ = NULL;
jmethodID AndroidJniClass_InputDevice::midGetVendorId_ = NULL;
jmethodID AndroidJniClass_InputDevice::midGetMotionRange_ = NULL;
jmethodID AndroidJniClass_InputDevice::midGetSources_ = NULL;
jmethodID AndroidJniClass_InputDevice::midHasKeys_ = NULL;
jclass AndroidJniClass_KeyCharacterMap::javaClass_ = NULL;
jmethodID AndroidJniClass_KeyCharacterMap::midDeviceHasKey_ = NULL;
jclass AndroidJniClass_MotionRange::javaClass_ = NULL;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AndroidJniHelper::attachJVM(struct android_app *state)
{
	javaVM_ = state->activity->vm;

	if (javaVM_ == NULL)
	{
		LOGE("JavaVM pointer is null");
	}
	else
	{
		int getEnvStatus = javaVM_->GetEnv(reinterpret_cast<void **>(&jniEnv_), JNI_VERSION_1_6);
		if (getEnvStatus == JNI_EDETACHED)
		{
			LOGW("GetEnv() cannot attach the JVM");
			if (javaVM_->AttachCurrentThread(&jniEnv_, NULL) != 0)
			{
				LOGW("AttachCurrentThread() cannot attach the JVM");
			}
			else
			{
				LOGI("AttachCurrentThread() successful");
			}
		}
		else if (getEnvStatus == JNI_EVERSION)
		{
			LOGW("GetEnv() with unsupported version");
		}
		else if (getEnvStatus == JNI_OK)
		{
			LOGI("GetEnv() successful");
		}

		if (jniEnv_ == NULL)
		{
			LOGE("JNIEnv pointer is null");
		}
		else
		{
			// Every JNI class will access the Java environment through this static pointer
			AndroidJniClass::jniEnv_ = jniEnv_;
			initClasses();

			// Cache the value of SDK version to avoid going through JNI in the future
			sdkVersion_ = AndroidJniClass_Version::sdkInt();
			LOGI_X("Android API version - NDK: %d, JNI: %d", __ANDROID_API__, sdkVersion_);
		}
	}
}

void AndroidJniHelper::detachJVM()
{
	if (javaVM_)
	{
		javaVM_->DetachCurrentThread();
		LOGI("Thread detached");
		javaVM_ = NULL;
		jniEnv_ = NULL;
	}
}

void AndroidJniHelper::initClasses()
{
	AndroidJniClass_Version::init();
	AndroidJniClass_InputDevice::init();
	AndroidJniClass_KeyCharacterMap::init();
}

void AndroidJniClass_Version::init()
{
	if (javaClass_ == NULL)
	{
		javaClass_ = jniEnv_->FindClass("android/os/Build$VERSION");
		if (javaClass_ == NULL)
		{
			LOGE("Cannot find class");
		}
		else
		{
			fidSdkInt_ = jniEnv_->GetStaticFieldID(javaClass_, "SDK_INT", "I");
			if (fidSdkInt_ == NULL)
			{
				LOGE("Cannot find static field SDK_INT");
			}
		}
	}
}

int AndroidJniClass_Version::sdkInt()
{
	jint sdkInt = jniEnv_->GetStaticIntField(javaClass_, fidSdkInt_);
	return sdkInt;
}

void AndroidJniClass_InputDevice::init()
{
	if (javaClass_ == NULL)
	{
		javaClass_ = jniEnv_->FindClass("android/view/InputDevice");
		if (javaClass_ == NULL)
		{
			LOGE("Cannot find class");
		}
		else
		{
			midGetDevice_ = jniEnv_->GetStaticMethodID(javaClass_, "getDevice", "(I)Landroid/view/InputDevice;");
			if (midGetDevice_ == NULL)
			{
				LOGE("Cannot find static method getDevice()");
			}

			midGetDeviceIds_ = jniEnv_->GetStaticMethodID(javaClass_, "getDeviceIds", "()[I");
			if (midGetDeviceIds_ == NULL)
			{
				LOGE("Cannot find static method getDeviceIds()");
			}

			midGetName_ = jniEnv_->GetMethodID(javaClass_, "getName", "()Ljava/lang/String;");
			if (midGetName_ == NULL)
			{
				LOGE("Cannot find method getName()");
			}

			midGetProductId_ = jniEnv_->GetMethodID(javaClass_, "getProductId", "()I");
			if (midGetProductId_ == NULL)
			{
				LOGE("Cannot find method getProductId()");
			}

			midGetVendorId_ = jniEnv_->GetMethodID(javaClass_, "getVendorId", "()I");
			if (midGetVendorId_ == NULL)
			{
				LOGE("Cannot find method getVendorId()");
			}

			midGetMotionRange_ = jniEnv_->GetMethodID(javaClass_, "getMotionRange", "(I)Landroid/view/InputDevice$MotionRange;");
			if (midGetMotionRange_ == NULL)
			{
				LOGE("Cannot find method getMotionRange()");
			}

			midGetSources_ = jniEnv_->GetMethodID(javaClass_, "getSources", "()I");
			if (midGetSources_ == NULL)
			{
				LOGE("Cannot find method getSources()");
			}

			midHasKeys_ = jniEnv_->GetMethodID(javaClass_, "hasKeys", "([I)[Z");
			if (midHasKeys_ == NULL)
			{
				LOGE("Cannot find method hasKeys()");
			}
		}
	}
}

AndroidJniClass_InputDevice AndroidJniClass_InputDevice::getDevice(int deviceId)
{
	jobject objInputDevice = jniEnv_->CallStaticObjectMethod(javaClass_, midGetDevice_, deviceId);
	return AndroidJniClass_InputDevice(objInputDevice);
}

int AndroidJniClass_InputDevice::getDeviceIds(int *destination, int maxSize)
{
	jintArray arrDeviceIds = static_cast<jintArray>(jniEnv_->CallStaticObjectMethod(javaClass_, midGetDeviceIds_));
	jint length = jniEnv_->GetArrayLength(arrDeviceIds);

	jint *intsDeviceIds = jniEnv_->GetIntArrayElements(arrDeviceIds, NULL);
	for (int i = 0; i < length && i < maxSize; i++)
	{
		destination[i] = int(intsDeviceIds[i]);
	}
	jniEnv_->ReleaseIntArrayElements(arrDeviceIds, intsDeviceIds, 0);
	jniEnv_->DeleteLocalRef(arrDeviceIds);

	return int(length);
}

void AndroidJniClass_InputDevice::getName(char *destination, int maxStringSize) const
{
	jstring strDeviceName = static_cast<jstring>(jniEnv_->CallObjectMethod(javaObject_, midGetName_));
	if (strDeviceName)
	{
		const char *deviceName = jniEnv_->GetStringUTFChars(strDeviceName, 0);
		strncpy(destination, deviceName, maxStringSize);
		destination[maxStringSize - 1] = '\0';
		jniEnv_->ReleaseStringUTFChars(strDeviceName, deviceName);
		jniEnv_->DeleteLocalRef(strDeviceName);
	}
	else
	{
		strncpy(destination, static_cast<const char *>("Unknown"), maxStringSize);
	}
}

int AndroidJniClass_InputDevice::getProductId() const
{
	// Early-out if SDK version requirements are not met
	if (AndroidJniHelper::sdkVersion() < 19 || __ANDROID_API__ < 19)
	{
		return 0;
	}

	jint productId = jniEnv_->CallIntMethod(javaObject_, midGetProductId_);
	return int(productId);
}

int AndroidJniClass_InputDevice::getVendorId() const
{
	// Early-out if SDK version requirements are not met
	if (AndroidJniHelper::sdkVersion() < 19 || __ANDROID_API__ < 19)
	{
		return 0;
	}

	jint vendorID = jniEnv_->CallIntMethod(javaObject_, midGetVendorId_);
	return int(vendorID);
}

AndroidJniClass_MotionRange AndroidJniClass_InputDevice::getMotionRange(int axis) const
{
	jobject objMotionRange = jniEnv_->CallObjectMethod(javaObject_, midGetMotionRange_, axis);
	return AndroidJniClass_MotionRange(objMotionRange);
}

int AndroidJniClass_InputDevice::getSources() const
{
	jint sources = jniEnv_->CallIntMethod(javaObject_, midGetSources_);
	return int(sources);
}

void AndroidJniClass_InputDevice::hasKeys(const int *buttons, const int length, bool *bools) const
{
	// Early-out if SDK version requirements are not met
	if (AndroidJniHelper::sdkVersion() < 19 || __ANDROID_API__ < 19)
	{
		return;
	}

	jintArray arrButtons = jniEnv_->NewIntArray(length);

	jint *intsButtons = jniEnv_->GetIntArrayElements(arrButtons, NULL);
	for (int i = 0; i < length; i++)
	{
		intsButtons[i] = buttons[i];
	}
	jniEnv_->ReleaseIntArrayElements(arrButtons, intsButtons, 0);

	jbooleanArray arrBooleans = static_cast<jbooleanArray>(jniEnv_->CallObjectMethod(javaObject_, midHasKeys_, arrButtons));
	jniEnv_->DeleteLocalRef(arrButtons);

	jboolean *booleans = jniEnv_->GetBooleanArrayElements(arrBooleans, NULL);
	for (int i = 0; i < length; i++)
	{
		bools[i] = bool(booleans[i]);
	}
	jniEnv_->ReleaseBooleanArrayElements(arrBooleans, booleans, 0);
	jniEnv_->DeleteLocalRef(arrBooleans);
}

void AndroidJniClass_KeyCharacterMap::init()
{
	if (javaClass_ == NULL)
	{
		javaClass_ = jniEnv_->FindClass("android/view/KeyCharacterMap");
		if (javaClass_ == NULL)
		{
			LOGE("Cannot find class");
		}
		else
		{
			midDeviceHasKey_ = jniEnv_->GetStaticMethodID(javaClass_, "deviceHasKey", "(I)Z");
			if (midDeviceHasKey_ == NULL)
			{
				LOGE("Cannot find static method deviceHasKey()");
			}
		}
	}
}

bool AndroidJniClass_KeyCharacterMap::deviceHasKey(int button)
{
	jboolean hasKey = jniEnv_->CallStaticBooleanMethod(javaClass_, midDeviceHasKey_, button);
	return (hasKey == JNI_TRUE);
}

AndroidJniClass_MotionRange::AndroidJniClass_MotionRange(jobject javaObject)
	: AndroidJniClass(javaObject)
{
	if (javaClass_ == NULL)
	{
		javaClass_ = jniEnv_->FindClass("android/view/InputDevice$MotionRange");
		if (javaClass_ == NULL)
		{
			LOGE("Cannot find class");
		}
	}
}

}
