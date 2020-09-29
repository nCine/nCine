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
	static void start(struct android_app *state, nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)());

	/// Processes an Android application command
	static void processCommand(struct android_app *state, int32_t cmd);

	/// Wrapper around `AndroidJniHelper::sdkVersion()`
	unsigned int sdkVersion() const;
	/// Wrapper around `AndroidInputManager::enableAccelerometer()`
	void enableAccelerometer(bool enabled);

	/// Returns true if the application has already called `init()`
	inline bool isInitialized() const { return isInitialized_; }

	/// Returns the path to the internal data directory from the native activity
	const char *internalDataPath() const;
	/// Returns the path to the external data directory from the native activity
	const char *externalDataPath() const;
	/// Returns the path to the OBB directory from the native activity
	const char *obbPath() const;

	/// Toggles the software keyboard
	void toggleSoftInput();

  private:
	bool isInitialized_;
	nctl::String packageName_;

	struct android_app *state_;
	nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler_)();
	void preInit();
	/// Must be called at the beginning to initialize the application
	void init();
	/// Must be called before exiting to shut down the application
	void shutdown();

	void setFocus(bool hasFocus) override;

	/// Private constructor
	AndroidApplication()
	    : Application(), isInitialized_(false), packageName_(128), state_(nullptr), createAppEventHandler_(nullptr) {}
	/// Private destructor
	~AndroidApplication() {}
	/// Deleted copy constructor
	AndroidApplication(const AndroidApplication &) = delete;
	/// Deleted assignment operator
	AndroidApplication &operator=(const AndroidApplication &) = delete;

	/// Returns the singleton reference to the Android application
	static AndroidApplication &theAndroidApplication() { return static_cast<AndroidApplication &>(theApplication()); }

	friend DLL_PUBLIC Application &theApplication();
};

/// Meyers' Singleton
DLL_PUBLIC Application &theApplication();

}

#endif
