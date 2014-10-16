#ifndef CLASS_NCANDROIDJNIHELPER
#define CLASS_NCANDROIDJNIHELPER

#include <jni.h>
#include <android/api-level.h>
#include <android_native_app_glue.h>

/// The class for calling the Java Android API via JNI
class ncAndroidJNIHelper
{
  public:
	inline static unsigned int sdkVersion() { return sdkVersion_; }

  private:
	static JavaVM *javaVM_;
	static JNIEnv *jniEnv_;

	static unsigned int sdkVersion_;

	static void attachJVM(struct android_app* state);
	static void detachJVM();
	static void initClasses();

	friend class ncApplication;
};

class ncAndroidJNIClass
{
  public:
	ncAndroidJNIClass() :  javaObject_(NULL) { }
	ncAndroidJNIClass(jobject javaObject) : javaObject_(javaObject) { }
	virtual ~ncAndroidJNIClass()
	{
		if (javaObject_)
		{
			jniEnv_->DeleteLocalRef(javaObject_);
		}
	}
	bool isNull() const { return javaObject_ == NULL; }

  protected:
	static JNIEnv *jniEnv_;
	jobject javaObject_;

	friend class ncAndroidJNIHelper;
};

class ncAndroidJNIClass_Version : public ncAndroidJNIClass
{
  public:
	static void init();
	static int sdkInt();

  private:
	static jclass javaClass_;
	static jfieldID fidSdkInt_;
};

class ncAndroidJNIClass_MotionRange : public ncAndroidJNIClass
{
  private:
	static jclass javaClass_;

  public:
	ncAndroidJNIClass_MotionRange(jobject javaObject);
};

class ncAndroidJNIClass_InputDevice : public ncAndroidJNIClass
{
  public:
	static void init();
	ncAndroidJNIClass_InputDevice(jobject javaObject)
		: ncAndroidJNIClass(javaObject) { }
	static ncAndroidJNIClass_InputDevice getDevice(int deviceId);
	static int getDeviceIds(int *destination, int maxSize);
	void getName(char *destination, int maxStringSize);
	ncAndroidJNIClass_MotionRange getMotionRange(int axis);
	int getSources();
	void hasKeys(int *buttons, const int length, bool *bools);

  private:
	static jclass javaClass_;
	static jmethodID midGetDevice_;
	static jmethodID midGetDeviceIds_;
	static jmethodID midGetName_;
	static jmethodID midGetMotionRange_;
	static jmethodID midGetSources_;
	static jmethodID midHasKeys_;
};

class ncAndroidJNIClass_KeyCharacterMap : public ncAndroidJNIClass
{
  public:
	static void init();
	ncAndroidJNIClass_KeyCharacterMap(jobject javaObject)
		: ncAndroidJNIClass(javaObject) { }
	static bool deviceHasKey(int button);

  private:
	static jclass javaClass_;
	static jmethodID midDeviceHasKey_;
};

#endif
