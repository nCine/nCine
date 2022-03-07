#ifndef CLASS_NCINE_SCREENVIEWPORT
#define CLASS_NCINE_SCREENVIEWPORT

#include "common_defines.h"
#include "Viewport.h"

namespace ncine {

/// The class handling the screen viewport
class ScreenViewport : public Viewport
{
  public:
	/// Creates the screen viewport
	ScreenViewport();

  private:
	void update();
	void visit();
	void sortAndCommitQueue();
	void draw();

	/// Deleted copy constructor
	ScreenViewport(const ScreenViewport &) = delete;
	/// Deleted assignment operator
	ScreenViewport &operator=(const ScreenViewport &) = delete;

	friend class Application;
};

}

#endif
