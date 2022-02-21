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
	static int start(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)(), int argc, char **argv);

  private:
	/// Suspension state from last frame
	bool wasSuspended_;

	/// A pointer to the custom Qt5 widget
	Qt5Widget *qt5Widget_;

	/// Must be called at the beginning to initialize the application
	void init(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)(), int argc, char **argv);
	/// Must be called continuously to keep the application running
	void run();
	/// Processes events inside the game loop
	void processEvents();
#ifdef __EMSCRIPTEN__
	static void emscriptenStep();
#endif

	/// Private constructor
	PCApplication()
	    : Application(), wasSuspended_(false), qt5Widget_(nullptr) {}
	/// Private destructor
	~PCApplication() = default;
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
