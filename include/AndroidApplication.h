#ifndef CLASS_NCINE_ANDROIDAPPLICATION
#define CLASS_NCINE_ANDROIDAPPLICATION

#include "Application.h"

struct android_app;

namespace ncine {

/// Main entry point and handler for nCine Android applications
class AndroidApplication : public Application
{
  public:
	static void preInit(IAppEventHandler* (*createAppEventHandler)());
	static void init(struct android_app* state);
	static void shutdown();

	inline static bool isInitialized() { return isInitialized_; }

	// Wrapper around EglGfxDevice::createSurface()
	static void createEglSurface(struct android_app* state);
	// Wrapper around EglGfxDevice::bind()
	static void bindEglContext();
	// Wrapper around EglGfxDevice::unbind()
	static void unbindEglContext();
	// Wrapper around EglGfxDevice::querySurfaceSize()
	static void queryEglSurfaceSize();

	static void setFocus(bool hasFocus);

  private:
	/// A flag indicating whether or not the application has already called init()
	static bool isInitialized_;

	AndroidApplication();
	~AndroidApplication();
	/// Private copy constructor
	AndroidApplication(const AndroidApplication&);
	/// Private assignment operator
	AndroidApplication& operator=(const AndroidApplication&);
};

}

#endif
