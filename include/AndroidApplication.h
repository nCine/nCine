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
	/// Entry point method to be called in the `android_main()` function
	static void start(struct android_app *state, IAppEventHandler * (*createAppEventHandler)());

	/// Processes an Android application command
	static void processCommand(struct android_app *state, int32_t cmd);

	/// Wrapper around `AndroidJniHelper::sdkVersion()`
	unsigned int sdkVersion() const;
	/// Wrapper around `AndroidInputManager::enableAccelerometer()`
	void enableAccelerometer(bool enabled);

	/// Returns true if the application has already called `init()`
	inline bool isInitialized() const { return isInitialized_; }

  private:
	bool isInitialized_;

	struct android_app *state_;
	IAppEventHandler *(*createAppEventHandler_)();
	void preInit();
	/// Must be called at the beginning to initialize the application
	void init();
	/// Must be called before exiting to shut down the application
	void shutdown();

	void setFocus(bool hasFocus);

	/// Private constructor
	AndroidApplication() : Application(),  isInitialized_(false), state_(NULL), createAppEventHandler_(NULL) { }
	/// Private destructor
	~AndroidApplication() { }
	/// Private copy constructor
	AndroidApplication(const AndroidApplication &);
	/// Private assignment operator
	AndroidApplication &operator=(const AndroidApplication &);

	/// Returns the singleton reference to the Android application
	static AndroidApplication &theAndroidApplication() { return static_cast<AndroidApplication &>(theApplication()); }

	friend DLL_PUBLIC Application &theApplication();
};

/// Meyers' Singleton
DLL_PUBLIC Application &theApplication();

}

#endif
