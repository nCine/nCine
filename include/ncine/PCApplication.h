#ifndef CLASS_NCINE_PCAPPLICATION
#define CLASS_NCINE_PCAPPLICATION

#include "Application.h"

namespace ncine {

class Qt5Widget;

/// Handler class for nCine applications on PC
class DLL_PUBLIC PCApplication : public Application
{
  public:
	/// Entry point method to be called in the `main()` function
	static int start(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)());

  private:
	/// Suspension state from last frame
	bool wasSuspended_;

#ifdef WITH_QT5
	Qt5Widget *widget_;
#endif

	/// Must be called at the beginning to initialize the application
	void init(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)());
	/// The main game loop
	void run();
	/// Processes events inside the game loop
	void processEvents();
#ifdef __EMSCRIPTEN__
	static void emscriptenStep();
#endif

	/// Private constructor
	PCApplication()
	    : Application() {}
	/// Private destructor
	~PCApplication() {}
	/// Deleted copy constructor
	PCApplication(const PCApplication &) = delete;
	/// Deleted assignment operator
	PCApplication &operator=(const PCApplication &) = delete;

	friend DLL_PUBLIC Application &theApplication();
	friend class Qt5Widget;
};

/// Meyers' Singleton
DLL_PUBLIC Application &theApplication();

}

#endif
