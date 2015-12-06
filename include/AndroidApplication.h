#ifndef CLASS_NCINE_ANDROIDAPPLICATION
#define CLASS_NCINE_ANDROIDAPPLICATION

#include "Application.h"

struct android_app;
struct AInputEvent;

namespace ncine {

/// Main entry point and handler for nCine Android applications
class DLL_PUBLIC AndroidApplication : public Application
{
  public:
	static void main(struct android_app* state, IAppEventHandler* (*createAppEventHandler)());

	// Processes an Android application command
	static void processCommand(struct android_app* state, int32_t cmd);

	// Wrapper around `AndroidJniHelper::sdkVersion()`
	unsigned int sdkVersion() const;
	// Wrapper around `AndroidInputManager::enableAccelerometer()`
	void enableAccelerometer(bool enabled);

	inline bool isInitialized() const { return isInitialized_; }

  private:
	/// A flag indicating whether or not the application has already called init()
	bool isInitialized_;

	struct android_app* state_;
	IAppEventHandler* (*createAppEventHandler_)();
	void preInit();
	void init();
	void shutdown();

	void setFocus(bool hasFocus);

	/// Private constructor
	AndroidApplication() : Application(),  isInitialized_(false), state_(NULL), createAppEventHandler_(NULL) { }
	/// Private destructor
	~AndroidApplication() { }
	/// Private copy constructor
	AndroidApplication(const AndroidApplication&);
	/// Private assignment operator
	AndroidApplication& operator=(const AndroidApplication&);

	static AndroidApplication& theAndroidApplication() { return static_cast<AndroidApplication &>(theApplication()); }

	friend DLL_PUBLIC Application& theApplication();
};

// Meyers' Singleton
DLL_PUBLIC Application& theApplication();

}

#endif
