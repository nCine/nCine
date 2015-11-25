#ifndef CLASS_NCINE_PCAPPLICATION
#define CLASS_NCINE_PCAPPLICATION

#include "Application.h"

namespace ncine {

/// Handler class for nCine applications on PC
class DLL_PUBLIC PCApplication : public Application
{
  public:
	static void main(IAppEventHandler* (*createAppEventHandler)());

  private:
	// Must be called at start to init the application
	void init(IAppEventHandler* (*createAppEventHandler)());
	// The main game loop, handling events and rendering
	void run();

	/// Private constructor
	PCApplication() : Application() { }
	/// Private destructor
	~PCApplication() { }
	/// Private copy constructor
	PCApplication(const PCApplication&);
	/// Private assignment operator
	PCApplication& operator=(const PCApplication&);

	friend DLL_PUBLIC Application& theApplication();
};

// Meyers' Singleton
DLL_PUBLIC Application& theApplication();

}

#endif
