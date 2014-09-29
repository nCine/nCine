#ifndef CLASS_NCANDROIDJNIHELPER
#define CLASS_NCANDROIDJNIHELPER

#include <jni.h>
#include <android/api-level.h>
#include <android_native_app_glue.h>

/// The class for calling the Java Android API via JNI
class ncAndroidJNIHelper
{
public:
	inline static unsigned int SDKVersion() { return s_uSDKVersion; }

private:
	static JavaVM *s_pJVM;
	static JNIEnv *s_pEnv;

	static void AttachJVM(struct android_app* state);
	static void DetachJVM();
	static void InitClasses();

	static unsigned int s_uSDKVersion;

	friend class ncApplication;
};

class ncAndroidJNIClass
{
protected:
	static JNIEnv *s_pEnv;
	jobject m_javaObject;

public:
	ncAndroidJNIClass() :  m_javaObject(NULL) { }
	ncAndroidJNIClass(jobject javaObject) : m_javaObject(javaObject) { }
	virtual ~ncAndroidJNIClass()
	{
		if (m_javaObject)
			s_pEnv->DeleteLocalRef(m_javaObject);
	}
	bool isNull() const { return m_javaObject == NULL; }

	friend class ncAndroidJNIHelper;
};

class ncAndroidJNIClass_Version : public ncAndroidJNIClass
{
private:
	static jclass s_javaClass;
	static jfieldID s_fidSDKINT;

public:
	static void Init();
	static int SDK_INT();
};

class ncAndroidJNIClass_MotionRange : public ncAndroidJNIClass
{
private:
	static jclass s_javaClass;

public:
	ncAndroidJNIClass_MotionRange(jobject javaObject);
};

class ncAndroidJNIClass_InputDevice : public ncAndroidJNIClass
{
private:
	static jclass s_javaClass;
	static jmethodID s_midGetDevice;
	static jmethodID s_midGetDeviceIds;
	static jmethodID s_midGetName;
	static jmethodID s_midGetMotionRange;
	static jmethodID s_midGetSources;
	static jmethodID s_midHasKeys;

public:
	static void Init();
	ncAndroidJNIClass_InputDevice(jobject javaObject)
		: ncAndroidJNIClass(javaObject) { }
	static ncAndroidJNIClass_InputDevice getDevice(int iDeviceId);
	static int getDeviceIds(int *vDestination, int iMaxSize);
	void getName(char *vDestination, int iMaxStringSize);
	ncAndroidJNIClass_MotionRange getMotionRange(int iAxis);
	int getSources();
	void hasKeys(int *vButtons, const int iLength, bool *vBools);
};

class ncAndroidJNIClass_KeyCharacterMap : public ncAndroidJNIClass
{
private:
	static jclass s_javaClass;
	static jmethodID s_midDeviceHasKey;

public:
	static void Init();
	ncAndroidJNIClass_KeyCharacterMap(jobject javaObject)
		: ncAndroidJNIClass(javaObject) { }
	static bool deviceHasKey(int iButton);
};

#endif
