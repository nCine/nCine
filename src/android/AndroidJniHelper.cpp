#include <cstring>
#include "common_macros.h"
#include "AndroidJniHelper.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

JavaVM *AndroidJniHelper::javaVM_ = nullptr;
JNIEnv *AndroidJniHelper::jniEnv_ = nullptr;
JNIEnv *AndroidJniClass::jniEnv_ = nullptr;
unsigned int AndroidJniHelper::sdkVersion_ = 0;

jclass AndroidJniClass_Version::javaClass_ = nullptr;
jfieldID AndroidJniClass_Version::fidSdkInt_ = nullptr;
jclass AndroidJniClass_InputDevice::javaClass_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetDevice_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetDeviceIds_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetName_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetDescriptor_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetProductId_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetVendorId_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetMotionRange_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetSources_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midHasKeys_ = nullptr;
jclass AndroidJniClass_KeyCharacterMap::javaClass_ = nullptr;
jmethodID AndroidJniClass_KeyCharacterMap::midDeviceHasKey_ = nullptr;
jclass AndroidJniClass_MotionRange::javaClass_ = nullptr;
jmethodID AndroidJniClass_MotionRange::midGetFlat_ = nullptr;
jmethodID AndroidJniClass_MotionRange::midGetFuzz_ = nullptr;
jmethodID AndroidJniClass_MotionRange::midGetMax_ = nullptr;
jmethodID AndroidJniClass_MotionRange::midGetMin_ = nullptr;
jmethodID AndroidJniClass_MotionRange::midGetRange_ = nullptr;
jclass AndroidJniClass_KeyEvent::javaClass_ = nullptr;
jmethodID AndroidJniClass_KeyEvent::midConstructor_ = nullptr;
jmethodID AndroidJniClass_KeyEvent::midGetUnicodeCharMetaState_ = nullptr;
jmethodID AndroidJniClass_KeyEvent::midGetUnicodeChar_ = nullptr;
jmethodID AndroidJniClass_KeyEvent::midIsPrintingKey_ = nullptr;

JNIEnv *AndroidJniWrap_Activity::jniEnv_ = nullptr;
jobject AndroidJniWrap_Activity::activityObject_ = nullptr;
jmethodID AndroidJniWrap_Activity::midFinishAndRemoveTask_ = nullptr;

JNIEnv *AndroidJniWrap_InputMethodManager::jniEnv_ = nullptr;
jobject AndroidJniWrap_InputMethodManager::inputMethodManagerObject_ = nullptr;
jmethodID AndroidJniWrap_InputMethodManager::midToggleSoftInput_ = nullptr;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

// ------------------- AndroidJniHelper -------------------

void AndroidJniHelper::attachJVM(struct android_app *state)
{
	javaVM_ = state->activity->vm;

	if (javaVM_ == nullptr)
		LOGE("JavaVM pointer is null");
	else
	{
		const int getEnvStatus = javaVM_->GetEnv(reinterpret_cast<void **>(&jniEnv_), JNI_VERSION_1_6);
		if (getEnvStatus == JNI_EDETACHED)
		{
			LOGW("GetEnv() cannot attach the JVM");
			if (javaVM_->AttachCurrentThread(&jniEnv_, nullptr) != 0)
				LOGW("AttachCurrentThread() cannot attach the JVM");
			else
				LOGI("AttachCurrentThread() successful");
		}
		else if (getEnvStatus == JNI_EVERSION)
			LOGW("GetEnv() with unsupported version");
		else if (getEnvStatus == JNI_OK)
			LOGI("GetEnv() successful");

		if (jniEnv_ == nullptr)
			LOGE("JNIEnv pointer is null");
		else
		{
			// Every JNI class will access the Java environment through this static pointer
			AndroidJniClass::jniEnv_ = jniEnv_;
			initClasses();
			AndroidJniWrap_Activity::init(jniEnv_, state);
			AndroidJniWrap_InputMethodManager::init(jniEnv_, state);

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
		AndroidJniClass::jniEnv_ = nullptr;
		javaVM_ = nullptr;
		jniEnv_ = nullptr;
	}
}

void AndroidJniHelper::initClasses()
{
	AndroidJniClass_Version::init();
	AndroidJniClass_MotionRange::init();
	AndroidJniClass_InputDevice::init();
	AndroidJniClass_KeyCharacterMap::init();
	AndroidJniClass_KeyEvent::init();
}

// ------------------- AndroidJniClass -------------------

AndroidJniClass::AndroidJniClass(jobject javaObject)
    : javaObject_(nullptr)
{
	FATAL_ASSERT(jniEnv_ != nullptr);
	if (javaObject)
	{
		javaObject_ = jniEnv_->NewGlobalRef(javaObject);
		jniEnv_->DeleteLocalRef(javaObject);
	}
}

AndroidJniClass::~AndroidJniClass()
{
	if (javaObject_)
		jniEnv_->DeleteGlobalRef(javaObject_);
}

AndroidJniClass::AndroidJniClass(AndroidJniClass &&other)
    : javaObject_(other.javaObject_)
{
	other.javaObject_ = nullptr;
}

AndroidJniClass &AndroidJniClass::operator=(AndroidJniClass &&other)
{
	nctl::swap(javaObject_, other.javaObject_);
	return *this;
}

jclass AndroidJniClass::findClass(const char *name)
{
	ASSERT(name != nullptr);
	jclass javaClass = jniEnv_->FindClass(name);
	if (javaClass == nullptr)
		LOGE_X("Cannot find Java class \"%s\"", name);

	return javaClass;
}

jmethodID AndroidJniClass::getStaticMethodID(jclass javaClass, const char *name, const char *signature)
{
	jmethodID mid = nullptr;

	ASSERT(name != nullptr && signature != nullptr);
	if (javaClass != nullptr)
	{
		mid = jniEnv_->GetStaticMethodID(javaClass, name, signature);
		if (mid == nullptr)
			LOGE_X("Cannot get static method \"%s()\" with signature \"%s\"", name, signature);
	}
	else
		LOGE("Cannot get static methods before finding the Java class");

	return mid;
}

jmethodID AndroidJniClass::getMethodID(jclass javaClass, const char *name, const char *signature)
{
	jmethodID mid = nullptr;

	ASSERT(name != nullptr && signature != nullptr);
	if (javaClass != nullptr)
	{
		mid = jniEnv_->GetMethodID(javaClass, name, signature);
		if (mid == nullptr)
			LOGE_X("Cannot get method \"%s()\" with signature \"%s\"", name, signature);
	}
	else
		LOGE("Cannot get methods before finding the Java class");

	return mid;
}

jfieldID AndroidJniClass::getStaticFieldID(jclass javaClass, const char *name, const char *signature)
{
	jfieldID fid = nullptr;

	ASSERT(name != nullptr && signature != nullptr);
	if (javaClass != nullptr)
	{
		fid = jniEnv_->GetStaticFieldID(javaClass, name, signature);
		if (fid == nullptr)
			LOGE_X("Cannot get static field \"%s\" with signature \"%s\"", name, signature);
	}
	else
		LOGE("Cannot get static fields before finding the Java class");

	return fid;
}

// ------------------- AndroidJniClass_Version -------------------

void AndroidJniClass_Version::init()
{
	javaClass_ = findClass("android/os/Build$VERSION");
	fidSdkInt_ = getStaticFieldID(javaClass_, "SDK_INT", "I");
}

int AndroidJniClass_Version::sdkInt()
{
	const jint sdkInt = jniEnv_->GetStaticIntField(javaClass_, fidSdkInt_);
	return int(sdkInt);
}

// ------------------- AndroidJniClass_InputDevice -------------------

void AndroidJniClass_InputDevice::init()
{
	javaClass_ = findClass("android/view/InputDevice");
	midGetDevice_ = getStaticMethodID(javaClass_, "getDevice", "(I)Landroid/view/InputDevice;");
	midGetDeviceIds_ = getStaticMethodID(javaClass_, "getDeviceIds", "()[I");
	midGetName_ = getMethodID(javaClass_, "getName", "()Ljava/lang/String;");
	midGetDescriptor_ = getMethodID(javaClass_, "getDescriptor", "()Ljava/lang/String;");
	midGetProductId_ = getMethodID(javaClass_, "getProductId", "()I");
	midGetVendorId_ = getMethodID(javaClass_, "getVendorId", "()I");
	midGetMotionRange_ = getMethodID(javaClass_, "getMotionRange", "(I)Landroid/view/InputDevice$MotionRange;");
	midGetSources_ = getMethodID(javaClass_, "getSources", "()I");
	midHasKeys_ = getMethodID(javaClass_, "hasKeys", "([I)[Z");
}

AndroidJniClass_InputDevice AndroidJniClass_InputDevice::getDevice(int deviceId)
{
	jobject inputDeviceObject = jniEnv_->CallStaticObjectMethod(javaClass_, midGetDevice_, deviceId);
	return AndroidJniClass_InputDevice(inputDeviceObject);
}

int AndroidJniClass_InputDevice::getDeviceIds(int *destination, int maxSize)
{
	jintArray arrDeviceIds = static_cast<jintArray>(jniEnv_->CallStaticObjectMethod(javaClass_, midGetDeviceIds_));
	const jint length = jniEnv_->GetArrayLength(arrDeviceIds);

	jint *intsDeviceIds = jniEnv_->GetIntArrayElements(arrDeviceIds, nullptr);
	for (int i = 0; i < length && i < maxSize; i++)
		destination[i] = int(intsDeviceIds[i]);
	jniEnv_->ReleaseIntArrayElements(arrDeviceIds, intsDeviceIds, 0);
	jniEnv_->DeleteLocalRef(arrDeviceIds);

	return int(length);
}

int AndroidJniClass_InputDevice::getName(char *destination, int maxStringSize) const
{
	jstring strDeviceName = static_cast<jstring>(jniEnv_->CallObjectMethod(javaObject_, midGetName_));
	const jsize length = jniEnv_->GetStringUTFLength(strDeviceName);

	if (strDeviceName)
	{
		const char *deviceName = jniEnv_->GetStringUTFChars(strDeviceName, 0);
		strncpy(destination, deviceName, maxStringSize);
		destination[maxStringSize - 1] = '\0';
		jniEnv_->ReleaseStringUTFChars(strDeviceName, deviceName);
		jniEnv_->DeleteLocalRef(strDeviceName);
	}
	else
		strncpy(destination, static_cast<const char *>("Unknown"), maxStringSize);

	return (int(length) < maxStringSize) ? int(length) : maxStringSize;
}

int AndroidJniClass_InputDevice::getDescriptor(char *destination, int maxStringSize) const
{
	jstring strDeviceDescriptor = static_cast<jstring>(jniEnv_->CallObjectMethod(javaObject_, midGetDescriptor_));
	const jsize length = jniEnv_->GetStringUTFLength(strDeviceDescriptor);

	if (strDeviceDescriptor)
	{
		const char *deviceDescriptor = jniEnv_->GetStringUTFChars(strDeviceDescriptor, 0);
		strncpy(destination, deviceDescriptor, maxStringSize);
		destination[maxStringSize - 1] = '\0';
		jniEnv_->ReleaseStringUTFChars(strDeviceDescriptor, deviceDescriptor);
		jniEnv_->DeleteLocalRef(strDeviceDescriptor);
	}
	else if (maxStringSize > 0)
		destination[0] = '\0';

	return (int(length) < maxStringSize) ? int(length) : maxStringSize;
}

int AndroidJniClass_InputDevice::getProductId() const
{
	// Early-out if SDK version requirements are not met
	if (AndroidJniHelper::sdkVersion() < 19)
		return 0;

	const jint productId = jniEnv_->CallIntMethod(javaObject_, midGetProductId_);
	return int(productId);
}

int AndroidJniClass_InputDevice::getVendorId() const
{
	// Early-out if SDK version requirements are not met
	if (AndroidJniHelper::sdkVersion() < 19)
		return 0;

	const jint vendorID = jniEnv_->CallIntMethod(javaObject_, midGetVendorId_);
	return int(vendorID);
}

AndroidJniClass_MotionRange AndroidJniClass_InputDevice::getMotionRange(int axis) const
{
	jobject motionRangeObject = jniEnv_->CallObjectMethod(javaObject_, midGetMotionRange_, axis);
	return AndroidJniClass_MotionRange(motionRangeObject);
}

int AndroidJniClass_InputDevice::getSources() const
{
	const jint sources = jniEnv_->CallIntMethod(javaObject_, midGetSources_);
	return int(sources);
}

void AndroidJniClass_InputDevice::hasKeys(const int *buttons, const int length, bool *bools) const
{
	// Early-out if SDK version requirements are not met
	if (AndroidJniHelper::sdkVersion() < 19 || __ANDROID_API__ < 19)
		return;

	jintArray arrButtons = jniEnv_->NewIntArray(length);

	jint *intsButtons = jniEnv_->GetIntArrayElements(arrButtons, nullptr);
	for (int i = 0; i < length; i++)
		intsButtons[i] = buttons[i];
	jniEnv_->ReleaseIntArrayElements(arrButtons, intsButtons, 0);

	jbooleanArray arrBooleans = static_cast<jbooleanArray>(jniEnv_->CallObjectMethod(javaObject_, midHasKeys_, arrButtons));
	jniEnv_->DeleteLocalRef(arrButtons);

	jboolean *booleans = jniEnv_->GetBooleanArrayElements(arrBooleans, nullptr);
	for (int i = 0; i < length; i++)
		bools[i] = bool(booleans[i]);
	jniEnv_->ReleaseBooleanArrayElements(arrBooleans, booleans, 0);
	jniEnv_->DeleteLocalRef(arrBooleans);
}

// ------------------- AndroidJniClass_KeyCharacterMap -------------------

void AndroidJniClass_KeyCharacterMap::init()
{
	javaClass_ = findClass("android/view/KeyCharacterMap");
	midDeviceHasKey_ = getStaticMethodID(javaClass_, "deviceHasKey", "(I)Z");
}

bool AndroidJniClass_KeyCharacterMap::deviceHasKey(int button)
{
	const jboolean hasKey = jniEnv_->CallStaticBooleanMethod(javaClass_, midDeviceHasKey_, button);
	return (hasKey == JNI_TRUE);
}

// ------------------- AndroidJniClass_MotionRange -------------------

void AndroidJniClass_MotionRange::init()
{
	javaClass_ = findClass("android/view/InputDevice$MotionRange");
	midGetFlat_ = getMethodID(javaClass_, "getFlat", "()F");
	midGetFuzz_ = getMethodID(javaClass_, "getFuzz", "()F");
	midGetMax_ = getMethodID(javaClass_, "getMax", "()F");
	midGetMin_ = getMethodID(javaClass_, "getMin", "()F");
	midGetRange_ = getMethodID(javaClass_, "getRange", "()F");
}

float AndroidJniClass_MotionRange::getFlat() const
{
	const jfloat flatValue = jniEnv_->CallFloatMethod(javaObject_, midGetFlat_);
	return float(flatValue);
}

float AndroidJniClass_MotionRange::getFuzz() const
{
	const jfloat fuzzValue = jniEnv_->CallFloatMethod(javaObject_, midGetFuzz_);
	return float(fuzzValue);
}

float AndroidJniClass_MotionRange::getMax() const
{
	const jfloat maxValue = jniEnv_->CallFloatMethod(javaObject_, midGetMax_);
	return float(maxValue);
}

float AndroidJniClass_MotionRange::getMin() const
{
	const jfloat minValue = jniEnv_->CallFloatMethod(javaObject_, midGetMin_);
	return float(minValue);
}

float AndroidJniClass_MotionRange::getRange() const
{
	const jfloat rangeValue = jniEnv_->CallFloatMethod(javaObject_, midGetRange_);
	return float(rangeValue);
}

// ------------------- AndroidJniClass_KeyEvent -------------------

void AndroidJniClass_KeyEvent::init()
{
	javaClass_ = findClass("android/view/KeyEvent");
	midConstructor_ = getMethodID(javaClass_, "<init>", "(II)V");
	midGetUnicodeCharMetaState_ = getMethodID(javaClass_, "getUnicodeChar", "(I)I");
	midGetUnicodeChar_ = getMethodID(javaClass_, "getUnicodeChar", "()I");
	midIsPrintingKey_ = getMethodID(javaClass_, "isPrintingKey", "()Z");
}

AndroidJniClass_KeyEvent::AndroidJniClass_KeyEvent(int action, int code)
{
	jobject javaObject = jniEnv_->NewObject(javaClass_, midConstructor_, action, code);
	javaObject_ = jniEnv_->NewGlobalRef(javaObject);
}

int AndroidJniClass_KeyEvent::getUnicodeChar(int metaState) const
{
	if (metaState != 0)
		return jniEnv_->CallIntMethod(javaObject_, midGetUnicodeCharMetaState_, metaState);
	else
		return jniEnv_->CallIntMethod(javaObject_, midGetUnicodeChar_);
}

bool AndroidJniClass_KeyEvent::isPrintingKey() const
{
	return jniEnv_->CallBooleanMethod(javaObject_, midIsPrintingKey_);
}

// ------------------- AndroidJniWrap_Activity -------------------

void AndroidJniWrap_Activity::init(JNIEnv *jniEnv, struct android_app *state)
{
	jniEnv_ = jniEnv;

	// Retrieve `NativeActivity`
	activityObject_ = state->activity->clazz;
	jclass nativeActivityClass = jniEnv_->GetObjectClass(activityObject_);

	midFinishAndRemoveTask_ = AndroidJniClass::getMethodID(nativeActivityClass, "finishAndRemoveTask", "()V");
}

void AndroidJniWrap_Activity::finishAndRemoveTask()
{
	// Check if SDK version requirements are met
	if (AndroidJniHelper::sdkVersion() >= 21)
		jniEnv_->CallVoidMethod(activityObject_, midFinishAndRemoveTask_);
}

// ------------------- AndroidJniWrap_InputMethodManager -------------------

void AndroidJniWrap_InputMethodManager::init(JNIEnv *jniEnv, struct android_app *state)
{
	jniEnv_ = jniEnv;

	// Retrieve `NativeActivity`
	jobject nativeActivityObject = state->activity->clazz;
	jclass nativeActivityClass = jniEnv_->GetObjectClass(nativeActivityObject);

	// Retrieve `Context.INPUT_METHOD_SERVICE`
	jclass contextClass = AndroidJniClass::findClass("android/content/Context");
	jfieldID fidInputMethodService = AndroidJniClass::getStaticFieldID(contextClass, "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
	jobject inputMethodServiceObject = jniEnv_->GetStaticObjectField(contextClass, fidInputMethodService);

	// Run `getSystemService(Context.INPUT_METHOD_SERVICE)`
	jclass inputMethodManagerClass = AndroidJniClass::findClass("android/view/inputmethod/InputMethodManager");
	jmethodID midGetSystemService = AndroidJniClass::getMethodID(nativeActivityClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	jobject inputMethodManagerObject = jniEnv_->CallObjectMethod(nativeActivityObject, midGetSystemService, inputMethodServiceObject);
	inputMethodManagerObject_ = jniEnv_->NewGlobalRef(inputMethodManagerObject);

	midToggleSoftInput_ = AndroidJniClass::getMethodID(inputMethodManagerClass, "toggleSoftInput", "(II)V");
}

void AndroidJniWrap_InputMethodManager::toggleSoftInput()
{
	jniEnv_->CallVoidMethod(inputMethodManagerObject_, midToggleSoftInput_, SHOW_IMPLICIT, HIDE_IMPLICIT_ONLY);
}

}
