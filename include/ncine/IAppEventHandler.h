#ifndef CLASS_NCINE_IAPPEVENTHANDLER
#define CLASS_NCINE_IAPPEVENTHANDLER

#include "common_defines.h"

namespace ncine {

class AppConfiguration;

/// The interface class for handling nCine application events
class DLL_PUBLIC IAppEventHandler
{
  public:
	/// Pure virtual destructor in order to make the class abstract
	virtual ~IAppEventHandler() = 0;

	/// Called once before initialization to setup the application
	/*! \warning At this stage it is only safe to modify the `config` object.
	 *  No other engine API calls should be made. */
	virtual void onPreInit(AppConfiguration &config) {}
	/// Called once on application initialization
	virtual void onInit() {}
	/// Called every time a frame is about to be rendered
	virtual void onFrameStart() {}
	/// Called every time after a frame has been rendered
	virtual void onFrameEnd() {}
	/// Called once on application shutdown
	virtual void onShutdown() {}
};

inline IAppEventHandler::~IAppEventHandler() {}

}

#endif
