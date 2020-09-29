#ifndef CLASS_NCINE_ANDROIDJNIHELPER
#define CLASS_NCINE_ANDROIDJNIHELPER

#include <jni.h>
#include <android/api-level.h>
#include <android_native_app_glue.h>

namespace ncine {

/// The class for setting up JNI and initialize requests classes
class AndroidJniHelper
{
  public:
	inline static unsigned int sdkVersion() { return sdkVersion_; }

  private:
	static JavaVM *javaVM_;
	static JNIEnv *jniEnv_;

	static unsigned int sdkVersion_;

	/// Attaches the Java virtual machine to make use of JNI
	static void attachJVM(struct android_app *state);
	/// Detaches the Java virtual machine
	static void detachJVM();
	static void initClasses();

	/// Static class, no constructor
	AndroidJniHelper();
	/// Static class, no copy constructor
	AndroidJniHelper(const AndroidJniHelper &other);
	/// Static class, no assignement operator
	AndroidJniHelper &operator=(const AndroidJniHelper &other);

	friend class AndroidApplication;
};

/// The base class for the classes handling JNI requests to the Android API
class AndroidJniClass
{
  public:
	AndroidJniClass()
	    : javaObject_(nullptr) {}
	explicit AndroidJniClass(jobject javaObject)
	    : javaObject_(javaObject) {}
	virtual ~AndroidJniClass()
	{
		if (javaObject_)
			jniEnv_->DeleteLocalRef(javaObject_);
	}
	inline bool isNull() const { return javaObject_ == nullptr; }

  protected:
	static JNIEnv *jniEnv_;
	jobject javaObject_;

	friend class AndroidJniHelper;
};

/// A class to handle JNI requests to `android.os.Build.VERSION`
class AndroidJniClass_Version : public AndroidJniClass
{
  public:
	static void init();
	static int sdkInt();

  private:
	static jclass javaClass_;
	static jfieldID fidSdkInt_;
};

/// A class to handle JNI requests to `android.view.InputDevice.MotionRange`
class AndroidJniClass_MotionRange : public AndroidJniClass
{
  private:
	static jclass javaClass_;

  public:
	explicit AndroidJniClass_MotionRange(jobject javaObject);
};

/// A class to handle JNI requests to `android.view.InputDevice`
class AndroidJniClass_InputDevice : public AndroidJniClass
{
  public:
	static void init();
	explicit AndroidJniClass_InputDevice(jobject javaObject)
	    : AndroidJniClass(javaObject) {}
	static AndroidJniClass_InputDevice getDevice(int deviceId);
	static int getDeviceIds(int *destination, int maxSize);
	void getName(char *destination, int maxStringSize) const;
	int getProductId() const;
	int getVendorId() const;
	AndroidJniClass_MotionRange getMotionRange(int axis) const;
	int getSources() const;
	void hasKeys(const int *buttons, const int length, bool *bools) const;

  private:
	static jclass javaClass_;
	static jmethodID midGetDevice_;
	static jmethodID midGetDeviceIds_;
	static jmethodID midGetName_;
	static jmethodID midGetVendorId_;
	static jmethodID midGetProductId_;
	static jmethodID midGetMotionRange_;
	static jmethodID midGetSources_;
	static jmethodID midHasKeys_;
};

/// A class to handle JNI requests to `android.view.KeyCharacterMap`
class AndroidJniClass_KeyCharacterMap : public AndroidJniClass
{
  public:
	static void init();
	explicit AndroidJniClass_KeyCharacterMap(jobject javaObject)
	    : AndroidJniClass(javaObject) {}
	static bool deviceHasKey(int button);

  private:
	static jclass javaClass_;
	static jmethodID midDeviceHasKey_;
};

/// A class to handle JNI requests to `android.view.KeyEvent`
class AndroidJniClass_KeyEvent : public AndroidJniClass
{
  public:
	static void init();

	AndroidJniClass_KeyEvent(int action, int code);
	int getUnicodeChar(int metaState);
	inline int getUnicodeChar() { return getUnicodeChar(0); }
	bool isPrintingKey();

  private:
	static jclass javaClass_;
	static jmethodID midConstructor_;
	static jmethodID midGetUnicodeCharMetaState_;
	static jmethodID midGetUnicodeChar_;
	static jmethodID midIsPrintingKey_;
};

/// A class to handle JNI requests to `android.view.inputmethod.InputMethodManager`
class AndroidJniWrap_InputMethodManager
{
  public:
	static void init(JNIEnv *jniEnv, struct android_app *state);
	static void toggleSoftInput();

  private:
	static JNIEnv *jniEnv_;
	static jobject inputMethodManagerObject_;
	static jmethodID midToggleSoftInput_;

	static const int SHOW_IMPLICIT = 1;
	static const int HIDE_IMPLICIT_ONLY = 1;
};

}

#endif
