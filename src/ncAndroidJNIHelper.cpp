#include "ncAndroidJNIHelper.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

JavaVM *ncAndroidJNIHelper::s_pJVM = NULL;
JNIEnv *ncAndroidJNIHelper::s_pEnv = NULL;
JNIEnv *ncAndroidJNIClass::s_pEnv = NULL;
unsigned int ncAndroidJNIHelper::s_uSDKVersion = 0;

jclass ncAndroidJNIClass_Version::s_javaClass = NULL;
jfieldID ncAndroidJNIClass_Version::s_fidSDKINT = NULL;
jclass ncAndroidJNIClass_InputDevice::s_javaClass = NULL;
jmethodID ncAndroidJNIClass_InputDevice::s_midGetDevice = NULL;
jmethodID ncAndroidJNIClass_InputDevice::s_midGetDeviceIds = NULL;
jmethodID ncAndroidJNIClass_InputDevice::s_midGetName = NULL;
jmethodID ncAndroidJNIClass_InputDevice::s_midGetMotionRange = NULL;
jmethodID ncAndroidJNIClass_InputDevice::s_midGetSources = NULL;
jmethodID ncAndroidJNIClass_InputDevice::s_midHasKeys = NULL;
jclass ncAndroidJNIClass_KeyCharacterMap::s_javaClass = NULL;
jmethodID ncAndroidJNIClass_KeyCharacterMap::s_midDeviceHasKey = NULL;
jclass ncAndroidJNIClass_MotionRange::s_javaClass = NULL;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncAndroidJNIHelper::AttachJVM(struct android_app* state)
{
	s_pJVM = state->activity->vm;

	if (s_pJVM == NULL)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIHelper::AttachJVM - JavaVM pointer is null");
	}
	else
	{
		int iGetEnvStatus = s_pJVM->GetEnv((void **)&s_pEnv, JNI_VERSION_1_6);
		if (iGetEnvStatus == JNI_EDETACHED)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncAndroidJNIHelper::AttachJVM - GetEnv() cannot attach the JVM");
			if (s_pJVM->AttachCurrentThread(&s_pEnv, NULL) != 0)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncAndroidJNIHelper::AttachJVM - AttachCurrentThread() cannot attach the JVM");
			}
			else
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAndroidJNIHelper::AttachJVM - AttachCurrentThread() successful");
			}
		}
		else if (iGetEnvStatus == JNI_EVERSION)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncAndroidJNIHelper::AttachJVM - GetEnv() with unsupported version");
		}
		else if (iGetEnvStatus == JNI_OK)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAndroidJNIHelper::AttachJVM - GetEnv() successful");
		}

		if (s_pEnv == NULL)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIHelper::AttachJVM - JNIEnv pointer is null");
		}
		else
		{
			// Every JNI class will access the Java environment through this static pointer
			ncAndroidJNIClass::s_pEnv = s_pEnv;
			InitClasses();

			// Cache the value of SDK version to avoid going through JNI in the future
			s_uSDKVersion = ncAndroidJNIClass_Version::SDK_INT();
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAndroidJNIHelper::AttachJVM - Android API version - NDK: %d, JNI: %d", __ANDROID_API__, s_uSDKVersion);
		}
	}
}

void ncAndroidJNIHelper::DetachJVM()
{
	if (s_pJVM)
	{
		s_pJVM->DetachCurrentThread();
		ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIHelper::DetachJVM - Thread detached");
		s_pJVM = NULL;
		s_pEnv = NULL;
	}
}

void ncAndroidJNIHelper::InitClasses()
{
	ncAndroidJNIClass_Version::Init();
	ncAndroidJNIClass_InputDevice::Init();
	ncAndroidJNIClass_KeyCharacterMap::Init();
}

void ncAndroidJNIClass_Version::Init()
{
	if (s_javaClass == NULL)
	{
		s_javaClass = s_pEnv->FindClass("android/os/Build$VERSION");
		if (s_javaClass == NULL)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_Version::Init - Cannot find class");
		}
		else
		{
			s_fidSDKINT = s_pEnv->GetStaticFieldID(s_javaClass, "SDK_INT", "I");
			if (s_fidSDKINT == NULL)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_Version::Init - Cannot find static field SDK_INT");
			}
		}
	}
}

int ncAndroidJNIClass_Version::SDK_INT()
{
	jint sdk_int = s_pEnv->GetStaticIntField(s_javaClass, s_fidSDKINT);
	return sdk_int;
}

void ncAndroidJNIClass_InputDevice::Init()
{
	if (s_javaClass == NULL)
	{
		s_javaClass = s_pEnv->FindClass("android/view/InputDevice");
		if (s_javaClass == NULL)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_InputDevice::Init - Cannot find class");
		}
		else
		{
			s_midGetDevice = s_pEnv->GetStaticMethodID(s_javaClass, "getDevice", "(I)Landroid/view/InputDevice;");
			if (s_midGetDevice == NULL)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_InputDevice::Init - Cannot find static method getDevice()");
			}

			s_midGetDeviceIds = s_pEnv->GetStaticMethodID(s_javaClass, "getDeviceIds", "()[I");
			if (s_midGetDeviceIds == NULL)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_InputDevice::Init - Cannot find static method getDeviceIds()");
			}

			s_midGetName = s_pEnv->GetMethodID(s_javaClass, "getName", "()Ljava/lang/String;");
			if (s_midGetName == NULL)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_InputDevice::Init - Cannot find method getName()");
			}

			s_midGetMotionRange = s_pEnv->GetMethodID(s_javaClass, "getMotionRange", "(I)Landroid/view/InputDevice$MotionRange;");
			if (s_midGetMotionRange == NULL)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_InputDevice::Init - Cannot find method getMotionRange()");
			}

			s_midGetSources = s_pEnv->GetMethodID(s_javaClass, "getSources", "()I");
			if (s_midGetSources == NULL)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_InputDevice::Init - Cannot find method getSources()");
			}

			s_midHasKeys = s_pEnv->GetMethodID(s_javaClass, "hasKeys", "([I)[Z");
			if (s_midHasKeys == NULL)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_InputDevice::Init - Cannot find method hasKeys()");
			}
		}
	}
}

ncAndroidJNIClass_InputDevice ncAndroidJNIClass_InputDevice::getDevice(int iDeviceId)
{
	jobject objInputDevice = s_pEnv->CallStaticObjectMethod(s_javaClass, s_midGetDevice, iDeviceId);
	return ncAndroidJNIClass_InputDevice(objInputDevice);
}

int ncAndroidJNIClass_InputDevice::getDeviceIds(int *vDestination, int iMaxSize)
{
	jintArray arrDeviceIds = (jintArray)s_pEnv->CallStaticObjectMethod(s_javaClass, s_midGetDeviceIds);
	jint length = s_pEnv->GetArrayLength(arrDeviceIds);

	jint *intsDeviceIds = s_pEnv->GetIntArrayElements(arrDeviceIds, NULL);
	for (int i = 0; i < length && i < iMaxSize; i++)
	{
		vDestination[i] = int(intsDeviceIds[i]);
	}
	s_pEnv->ReleaseIntArrayElements(arrDeviceIds, intsDeviceIds, 0);

	return int(length);
}

void ncAndroidJNIClass_InputDevice::getName(char *vDestination, int iMaxStringSize)
{
	jstring strDeviceName = (jstring)s_pEnv->CallObjectMethod(m_javaObject, s_midGetName);
	if (strDeviceName)
	{
		const char *vDeviceName = s_pEnv->GetStringUTFChars(strDeviceName, 0);
		strncpy(vDestination, vDeviceName, iMaxStringSize);
		vDestination[iMaxStringSize - 1] = '\0';
		s_pEnv->ReleaseStringUTFChars(strDeviceName, vDeviceName);
		s_pEnv->DeleteLocalRef(strDeviceName);
	}
	else
	{
		strncpy(vDestination, (const char *)"Unknown", iMaxStringSize);
	}
}

ncAndroidJNIClass_MotionRange ncAndroidJNIClass_InputDevice::getMotionRange(int iAxis)
{
	jobject objMotionRange = s_pEnv->CallObjectMethod(m_javaObject, s_midGetMotionRange, iAxis);
	return ncAndroidJNIClass_MotionRange(objMotionRange);
}

int ncAndroidJNIClass_InputDevice::getSources()
{
	jint sources = s_pEnv->CallIntMethod(m_javaObject, s_midGetSources);
	return int(sources);
}

void ncAndroidJNIClass_InputDevice::hasKeys(int *vButtons, const int iLength, bool *vBools)
{
	// Early-out if SDK version requirements are not met
	if (ncAndroidJNIHelper::SDKVersion() < 19 || __ANDROID_API__ < 19)
	{
		return;
	}

	jintArray arrButtons = s_pEnv->NewIntArray(iLength);

	jint *intsButtons = s_pEnv->GetIntArrayElements(arrButtons, NULL);
	for (int i = 0; i < iLength; i++)
	{
		intsButtons[i] = vButtons[i];
	}
	s_pEnv->ReleaseIntArrayElements(arrButtons, intsButtons, 0);

	jbooleanArray arrBooleans = (jbooleanArray)s_pEnv->CallObjectMethod(m_javaObject, s_midHasKeys, arrButtons);
	int length = s_pEnv->GetArrayLength(arrBooleans);

	jboolean *booleans = s_pEnv->GetBooleanArrayElements(arrBooleans, NULL);
	for (int i = 0; i < length; i++)
	{
		vBools[i] = bool(booleans[i]);
	}
	s_pEnv->ReleaseBooleanArrayElements(arrBooleans, booleans, 0);
}

void ncAndroidJNIClass_KeyCharacterMap::Init()
{
	if (s_javaClass == NULL)
	{
		s_javaClass = s_pEnv->FindClass("android/view/KeyCharacterMap");
		if (s_javaClass == NULL)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_KeyCharacterMap::Init - Cannot find class");
		}
		else
		{
			s_midDeviceHasKey = s_pEnv->GetStaticMethodID(s_javaClass, "deviceHasKey", "(I)Z");
			if (s_midDeviceHasKey == NULL)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_KeyCharacterMap::Init - Cannot find static method deviceHasKey()");
			}
		}
	}
}

bool ncAndroidJNIClass_KeyCharacterMap::deviceHasKey(int iButton)
{
	jboolean hasKey = s_pEnv->CallStaticBooleanMethod(s_javaClass, s_midDeviceHasKey, iButton);
	return (hasKey == JNI_TRUE);
}

ncAndroidJNIClass_MotionRange::ncAndroidJNIClass_MotionRange(jobject javaObject)
	: ncAndroidJNIClass(javaObject)
{
	if (s_javaClass == NULL)
	{
		s_javaClass = s_pEnv->FindClass("android/view/InputDevice$MotionRange");
		if (s_javaClass == NULL)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidJNIClass_MotionRange::ncAndroidJNIClass_MotionRange - Cannot find class");
		}
	}
}
