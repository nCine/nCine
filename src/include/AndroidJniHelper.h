#ifndef CLASS_NCINE_ANDROIDJNIHELPER
#define CLASS_NCINE_ANDROIDJNIHELPER

#include <jni.h>
#include <android/api-level.h>
#include <android_native_app_glue.h>

namespace ncine {

/// The class for calling the Java Android API via JNI
class AndroidJniHelper
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

	/// Static class, no constructor
	AndroidJniHelper();
	/// Static class, no copy constructor
	AndroidJniHelper(const AndroidJniHelper& other);
	/// Static class, no assignement operator
	AndroidJniHelper& operator=(const AndroidJniHelper& other);

	friend class AndroidApplication;
};

class AndroidJniClass
{
  public:
	AndroidJniClass() :  javaObject_(NULL) { }
	AndroidJniClass(jobject javaObject) : javaObject_(javaObject) { }
	virtual ~AndroidJniClass()
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

	friend class AndroidJniHelper;
};

class AndroidJniClass_Version : public AndroidJniClass
{
  public:
	static void init();
	static int sdkInt();

  private:
	static jclass javaClass_;
	static jfieldID fidSdkInt_;
};

class AndroidJniClass_MotionRange : public AndroidJniClass
{
  private:
	static jclass javaClass_;

  public:
	AndroidJniClass_MotionRange(jobject javaObject);
};

class AndroidJniClass_InputDevice : public AndroidJniClass
{
  public:
	static void init();
	AndroidJniClass_InputDevice(jobject javaObject)
		: AndroidJniClass(javaObject) { }
	static AndroidJniClass_InputDevice getDevice(int deviceId);
	static int getDeviceIds(int *destination, int maxSize);
	void getName(char *destination, int maxStringSize);
	AndroidJniClass_MotionRange getMotionRange(int axis);
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

class AndroidJniClass_KeyCharacterMap : public AndroidJniClass
{
  public:
	static void init();
	AndroidJniClass_KeyCharacterMap(jobject javaObject)
		: AndroidJniClass(javaObject) { }
	static bool deviceHasKey(int button);

  private:
	static jclass javaClass_;
	static jmethodID midDeviceHasKey_;
};

}

#endif
