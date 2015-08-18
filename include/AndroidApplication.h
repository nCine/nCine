#ifndef CLASS_NCINE_ANDROIDAPPLICATION
#define CLASS_NCINE_ANDROIDAPPLICATION

#include "Application.h"

struct android_app;

namespace ncine {

/// Main entry point and handler for nCine Android applications
class DLL_PUBLIC AndroidApplication : public Application
{
  public:
	void preInit(IAppEventHandler* (*createAppEventHandler)());
	void init(struct android_app* state);
	void shutdown();

	// Wrapper around AndroidJniHelper::sdkVersion()
	unsigned int sdkVersion() const;

	inline bool isInitialized() const { return isInitialized_; }

	void setFocus(bool hasFocus);

  private:
	/// A flag indicating whether or not the application has already called init()
	static bool isInitialized_;

	AndroidApplication() : Application() { }
	~AndroidApplication() { }
	/// Private copy constructor
	AndroidApplication(const AndroidApplication&);
	/// Private assignment operator
	AndroidApplication& operator=(const AndroidApplication&);

	friend AndroidApplication& theApplication();
};

// Meyers' Singleton
DLL_PUBLIC AndroidApplication& theApplication();

}

#endif
