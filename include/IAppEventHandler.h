#ifndef CLASS_NCINE_IAPPEVENTHANDLER
#define CLASS_NCINE_IAPPEVENTHANDLER

namespace ncine {

class AppConfiguration;

/// The interface class for handling application events (i.e. the game)
class IAppEventHandler
{
  public:
	/// Pure virtual destructor in order to make the class abstract
	virtual ~IAppEventHandler() = 0;

	/// Called once before initialization to setup the application
	virtual void onPreInit(AppConfiguration& config) { }
	/// Called once on application init
	virtual void onInit() { }
	/// Called every time a frame is about to be rendered
	virtual void onFrameStart() { }
	/// Called every time after a frame has been rendered
	virtual void onFrameEnd() { }
	/// Called once on application shutdown
	virtual void onShutdown() { }
};

inline IAppEventHandler::~IAppEventHandler() { }

}

#endif
