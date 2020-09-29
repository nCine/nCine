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
jmethodID AndroidJniClass_InputDevice::midGetProductId_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetVendorId_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetMotionRange_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midGetSources_ = nullptr;
jmethodID AndroidJniClass_InputDevice::midHasKeys_ = nullptr;
jclass AndroidJniClass_KeyCharacterMap::javaClass_ = nullptr;
jmethodID AndroidJniClass_KeyCharacterMap::midDeviceHasKey_ = nullptr;
jclass AndroidJniClass_MotionRange::javaClass_ = nullptr;
jclass AndroidJniClass_KeyEvent::javaClass_ = nullptr;
jmethodID AndroidJniClass_KeyEvent::midConstructor_ = nullptr;
jmethodID AndroidJniClass_KeyEvent::midGetUnicodeCharMetaState_ = nullptr;
jmethodID AndroidJniClass_KeyEvent::midGetUnicodeChar_ = nullptr;
jmethodID AndroidJniClass_KeyEvent::midIsPrintingKey_ = nullptr;

JNIEnv *AndroidJniWrap_InputMethodManager::jniEnv_ = nullptr;
jobject AndroidJniWrap_InputMethodManager::inputMethodManagerObject_ = nullptr;
jmethodID AndroidJniWrap_InputMethodManager::midToggleSoftInput_ = nullptr;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AndroidJniHelper::attachJVM(struct android_app *state)
{
	javaVM_ = state->activity->vm;

	if (javaVM_ == nullptr)
		LOGE("JavaVM pointer is null");
	else
	{
		int getEnvStatus = javaVM_->GetEnv(reinterpret_cast<void **>(&jniEnv_), JNI_VERSION_1_6);
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
	AndroidJniClass_InputDevice::init();
	AndroidJniClass_KeyCharacterMap::init();
	AndroidJniClass_KeyEvent::init();
}

void AndroidJniClass_Version::init()
{
	if (javaClass_ == nullptr)
	{
		javaClass_ = jniEnv_->FindClass("android/os/Build$VERSION");
		if (javaClass_ == nullptr)
			LOGE("Cannot find class");
		else
		{
			fidSdkInt_ = jniEnv_->GetStaticFieldID(javaClass_, "SDK_INT", "I");
			if (fidSdkInt_ == nullptr)
				LOGE("Cannot find static field SDK_INT");
		}
	}
}

int AndroidJniClass_Version::sdkInt()
{
	const jint sdkInt = jniEnv_->GetStaticIntField(javaClass_, fidSdkInt_);
	return int(sdkInt);
}

void AndroidJniClass_InputDevice::init()
{
	if (javaClass_ == nullptr)
	{
		javaClass_ = jniEnv_->FindClass("android/view/InputDevice");
		if (javaClass_ == nullptr)
			LOGE("Cannot find class");
		else
		{
			midGetDevice_ = jniEnv_->GetStaticMethodID(javaClass_, "getDevice", "(I)Landroid/view/InputDevice;");
			if (midGetDevice_ == nullptr)
				LOGE("Cannot find static method getDevice()");

			midGetDeviceIds_ = jniEnv_->GetStaticMethodID(javaClass_, "getDeviceIds", "()[I");
			if (midGetDeviceIds_ == nullptr)
				LOGE("Cannot find static method getDeviceIds()");

			midGetName_ = jniEnv_->GetMethodID(javaClass_, "getName", "()Ljava/lang/String;");
			if (midGetName_ == nullptr)
				LOGE("Cannot find method getName()");

			midGetProductId_ = jniEnv_->GetMethodID(javaClass_, "getProductId", "()I");
			if (midGetProductId_ == nullptr)
				LOGE("Cannot find method getProductId()");

			midGetVendorId_ = jniEnv_->GetMethodID(javaClass_, "getVendorId", "()I");
			if (midGetVendorId_ == nullptr)
				LOGE("Cannot find method getVendorId()");

			midGetMotionRange_ = jniEnv_->GetMethodID(javaClass_, "getMotionRange", "(I)Landroid/view/InputDevice$MotionRange;");
			if (midGetMotionRange_ == nullptr)
				LOGE("Cannot find method getMotionRange()");

			midGetSources_ = jniEnv_->GetMethodID(javaClass_, "getSources", "()I");
			if (midGetSources_ == nullptr)
				LOGE("Cannot find method getSources()");

			midHasKeys_ = jniEnv_->GetMethodID(javaClass_, "hasKeys", "([I)[Z");
			if (midHasKeys_ == nullptr)
				LOGE("Cannot find method hasKeys()");
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
	const jint length = jniEnv_->GetArrayLength(arrDeviceIds);

	jint *intsDeviceIds = jniEnv_->GetIntArrayElements(arrDeviceIds, nullptr);
	for (int i = 0; i < length && i < maxSize; i++)
		destination[i] = int(intsDeviceIds[i]);
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
		strncpy(destination, static_cast<const char *>("Unknown"), maxStringSize);
}

int AndroidJniClass_InputDevice::getProductId() const
{
	// Early-out if SDK version requirements are not met
	if (AndroidJniHelper::sdkVersion() < 19 || __ANDROID_API__ < 19)
		return 0;

	const jint productId = jniEnv_->CallIntMethod(javaObject_, midGetProductId_);
	return int(productId);
}

int AndroidJniClass_InputDevice::getVendorId() const
{
	// Early-out if SDK version requirements are not met
	if (AndroidJniHelper::sdkVersion() < 19 || __ANDROID_API__ < 19)
		return 0;

	const jint vendorID = jniEnv_->CallIntMethod(javaObject_, midGetVendorId_);
	return int(vendorID);
}

AndroidJniClass_MotionRange AndroidJniClass_InputDevice::getMotionRange(int axis) const
{
	jobject objMotionRange = jniEnv_->CallObjectMethod(javaObject_, midGetMotionRange_, axis);
	return AndroidJniClass_MotionRange(objMotionRange);
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

void AndroidJniClass_KeyCharacterMap::init()
{
	if (javaClass_ == nullptr)
	{
		javaClass_ = jniEnv_->FindClass("android/view/KeyCharacterMap");
		if (javaClass_ == nullptr)
			LOGE("Cannot find class");
		else
		{
			midDeviceHasKey_ = jniEnv_->GetStaticMethodID(javaClass_, "deviceHasKey", "(I)Z");
			if (midDeviceHasKey_ == nullptr)
				LOGE("Cannot find static method deviceHasKey()");
		}
	}
}

bool AndroidJniClass_KeyCharacterMap::deviceHasKey(int button)
{
	const jboolean hasKey = jniEnv_->CallStaticBooleanMethod(javaClass_, midDeviceHasKey_, button);
	return (hasKey == JNI_TRUE);
}

AndroidJniClass_MotionRange::AndroidJniClass_MotionRange(jobject javaObject)
    : AndroidJniClass(javaObject)
{
	if (javaClass_ == nullptr)
	{
		javaClass_ = jniEnv_->FindClass("android/view/InputDevice$MotionRange");
		if (javaClass_ == nullptr)
			LOGE("Cannot find class");
	}
}

void AndroidJniClass_KeyEvent::init()
{
	if (javaClass_ == nullptr)
	{
		javaClass_ = jniEnv_->FindClass("android/view/KeyEvent");
		if (javaClass_ == nullptr)
			LOGE("Cannot find class");
		else
		{
			midConstructor_ = jniEnv_->GetMethodID(javaClass_, "<init>", "(II)V");
			if (midConstructor_ == nullptr)
				LOGE("Cannot find KeyEvent(int, int) constructor");

			midGetUnicodeCharMetaState_ = jniEnv_->GetMethodID(javaClass_, "getUnicodeChar", "(I)I");
			if (midGetUnicodeCharMetaState_ == nullptr)
				LOGE("Cannot find method getUnicodeChar(int)");

			midGetUnicodeChar_ = jniEnv_->GetMethodID(javaClass_, "getUnicodeChar", "()I");
			if (midGetUnicodeChar_ == nullptr)
				LOGE("Cannot find method getUnicodeChar()");

			midIsPrintingKey_ = jniEnv_->GetMethodID(javaClass_, "isPrintingKey", "()Z");
			if (midIsPrintingKey_ == nullptr)
				LOGE("Cannot find method isPrintingKey()");
		}
	}
}

AndroidJniClass_KeyEvent::AndroidJniClass_KeyEvent(int action, int code)
{
	javaObject_ = jniEnv_->NewObject(javaClass_, midConstructor_, action, code);
}

int AndroidJniClass_KeyEvent::getUnicodeChar(int metaState)
{
	if (metaState != 0)
		return jniEnv_->CallIntMethod(javaObject_, midGetUnicodeCharMetaState_, metaState);
	else
		return jniEnv_->CallIntMethod(javaObject_, midGetUnicodeChar_);
}

bool AndroidJniClass_KeyEvent::isPrintingKey()
{
	return jniEnv_->CallBooleanMethod(javaObject_, midIsPrintingKey_);
}

void AndroidJniWrap_InputMethodManager::init(JNIEnv *jniEnv, struct android_app *state)
{
	jniEnv_ = jniEnv;

	// Retrieve `NativeActivity`
	jobject nativeActivityObject = state->activity->clazz;
	jclass nativeActivityClass = jniEnv->GetObjectClass(nativeActivityObject);

	// Retrieve `Context.INPUT_METHOD_SERVICE`
	jclass contextClass = jniEnv->FindClass("android/content/Context");
	jfieldID fidInputMethodService = jniEnv->GetStaticFieldID(contextClass, "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
	jobject inputMethodServiceObject = jniEnv->GetStaticObjectField(contextClass, fidInputMethodService);

	// Run `getSystemService(Context.INPUT_METHOD_SERVICE)`
	jclass inputMethodManagerClass = jniEnv->FindClass("android/view/inputmethod/InputMethodManager");
	if (inputMethodManagerClass == nullptr)
		LOGE("Cannot find class");
	jmethodID midGetSystemService = jniEnv->GetMethodID(nativeActivityClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	inputMethodManagerObject_ = jniEnv->CallObjectMethod(nativeActivityObject, midGetSystemService, inputMethodServiceObject);

	midToggleSoftInput_ = jniEnv->GetMethodID(inputMethodManagerClass, "toggleSoftInput", "(II)V");
	if (midToggleSoftInput_ == nullptr)
		LOGE("Cannot find method toggleSoftInput()");
}

void AndroidJniWrap_InputMethodManager::toggleSoftInput()
{
	jniEnv_->CallVoidMethod(inputMethodManagerObject_, midToggleSoftInput_, SHOW_IMPLICIT, HIDE_IMPLICIT_ONLY);
}

}
