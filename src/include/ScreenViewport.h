#ifndef CLASS_NCINE_SCREENVIEWPORT
#define CLASS_NCINE_SCREENVIEWPORT

#include "common_defines.h"
#include "Viewport.h"

namespace ncine {

/// The class handling the root viewport of the screen
class ScreenViewport : public Viewport
{
  public:
	/// Creates the screen viewport
	ScreenViewport();
	~ScreenViewport() {}

	inline bool isScreen() const { return true; }

  private:
	void update();
	void visit();
	void sortAndCommitQueue();
	void draw();

	friend class Application;
};

}

#endif
