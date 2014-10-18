#include "ncAndroidJNIHelper.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

JavaVM *ncAndroidJNIHelper::javaVM_ = NULL;
JNIEnv *ncAndroidJNIHelper::jniEnv_ = NULL;
JNIEnv *ncAndroidJNIClass::jniEnv_ = NULL;
unsigned int ncAndroidJNIHelper::sdkVersion_ = 0;

jclass ncAndroidJNIClass_Version::javaClass_ = NULL;
jfieldID ncAndroidJNIClass_Version::fidSdkInt_ = NULL;
jclass ncAndroidJNIClass_InputDevice::javaClass_ = NULL;
jmethodID ncAndroidJNIClass_InputDevice::midGetDevice_ = NULL;
jmethodID ncAndroidJNIClass_InputDevice::midGetDeviceIds_ = NULL;
jmethodID ncAndroidJNIClass_InputDevice::midGetName_ = NULL;
jmethodID ncAndroidJNIClass_InputDevice::midGetMotionRange_ = NULL;
jmethodID ncAndroidJNIClass_InputDevice::midGetSources_ = NULL;
jmethodID ncAndroidJNIClass_InputDevice::midHasKeys_ = NULL;
jclass ncAndroidJNIClass_KeyCharacterMap::javaClass_ = NULL;
jmethodID ncAndroidJNIClass_KeyCharacterMap::midDeviceHasKey_ = NULL;
jclass ncAndroidJNIClass_MotionRange::javaClass_ = NULL;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncAndroidJNIHelper::attachJVM(struct android_app* state)
{
	javaVM_ = state->activity->vm;

	if (javaVM_ == NULL)
	{
		LOGE("JavaVM pointer is null");
	}
	else
	{
		int getEnvStatus = javaVM_->GetEnv((void **)&jniEnv_, JNI_VERSION_1_6);
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
			ncAndroidJNIClass::jniEnv_ = jniEnv_;
			initClasses();

			// Cache the value of SDK version to avoid going through JNI in the future
			sdkVersion_ = ncAndroidJNIClass_Version::sdkInt();
			LOGI_X("Android API version - NDK: %d, JNI: %d", __ANDROID_API__, sdkVersion_);
		}
	}
}

void ncAndroidJNIHelper::detachJVM()
{
	if (javaVM_)
	{
		javaVM_->DetachCurrentThread();
		LOGI("Thread detached");
		javaVM_ = NULL;
		jniEnv_ = NULL;
	}
}

void ncAndroidJNIHelper::initClasses()
{
	ncAndroidJNIClass_Version::init();
	ncAndroidJNIClass_InputDevice::init();
	ncAndroidJNIClass_KeyCharacterMap::init();
}

void ncAndroidJNIClass_Version::init()
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

int ncAndroidJNIClass_Version::sdkInt()
{
	jint sdkInt = jniEnv_->GetStaticIntField(javaClass_, fidSdkInt_);
	return sdkInt;
}

void ncAndroidJNIClass_InputDevice::init()
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

ncAndroidJNIClass_InputDevice ncAndroidJNIClass_InputDevice::getDevice(int deviceId)
{
	jobject objInputDevice = jniEnv_->CallStaticObjectMethod(javaClass_, midGetDevice_, deviceId);
	return ncAndroidJNIClass_InputDevice(objInputDevice);
}

int ncAndroidJNIClass_InputDevice::getDeviceIds(int *destination, int maxSize)
{
	jintArray arrDeviceIds = (jintArray)jniEnv_->CallStaticObjectMethod(javaClass_, midGetDeviceIds_);
	jint length = jniEnv_->GetArrayLength(arrDeviceIds);

	jint *intsDeviceIds = jniEnv_->GetIntArrayElements(arrDeviceIds, NULL);
	for (int i = 0; i < length && i < maxSize; i++)
	{
		destination[i] = int(intsDeviceIds[i]);
	}
	jniEnv_->ReleaseIntArrayElements(arrDeviceIds, intsDeviceIds, 0);

	return int(length);
}

void ncAndroidJNIClass_InputDevice::getName(char *destination, int maxStringSize)
{
	jstring strDeviceName = (jstring)jniEnv_->CallObjectMethod(javaObject_, midGetName_);
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
		strncpy(destination, (const char *)"Unknown", maxStringSize);
	}
}

ncAndroidJNIClass_MotionRange ncAndroidJNIClass_InputDevice::getMotionRange(int axis)
{
	jobject objMotionRange = jniEnv_->CallObjectMethod(javaObject_, midGetMotionRange_, axis);
	return ncAndroidJNIClass_MotionRange(objMotionRange);
}

int ncAndroidJNIClass_InputDevice::getSources()
{
	jint sources = jniEnv_->CallIntMethod(javaObject_, midGetSources_);
	return int(sources);
}

void ncAndroidJNIClass_InputDevice::hasKeys(int *buttons, const int length, bool *bools)
{
	// Early-out if SDK version requirements are not met
	if (ncAndroidJNIHelper::sdkVersion() < 19 || __ANDROID_API__ < 19)
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

	jbooleanArray arrBooleans = (jbooleanArray)jniEnv_->CallObjectMethod(javaObject_, midHasKeys_, arrButtons);

	jboolean *booleans = jniEnv_->GetBooleanArrayElements(arrBooleans, NULL);
	for (int i = 0; i < length; i++)
	{
		bools[i] = bool(booleans[i]);
	}
	jniEnv_->ReleaseBooleanArrayElements(arrBooleans, booleans, 0);
}

void ncAndroidJNIClass_KeyCharacterMap::init()
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

bool ncAndroidJNIClass_KeyCharacterMap::deviceHasKey(int button)
{
	jboolean hasKey = jniEnv_->CallStaticBooleanMethod(javaClass_, midDeviceHasKey_, button);
	return (hasKey == JNI_TRUE);
}

ncAndroidJNIClass_MotionRange::ncAndroidJNIClass_MotionRange(jobject javaObject)
	: ncAndroidJNIClass(javaObject)
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
