#ifndef CLASS_NCINE_IAPPEVENTHANDLER
#define CLASS_NCINE_IAPPEVENTHANDLER

namespace ncine {

/// The interface class for handling application events (i.e. the game)
class IAppEventHandler
{
  public:
	virtual ~IAppEventHandler() { }

	/// Called once on application init
	virtual void onInit() = 0;
	/// Called every time a frame is about to be rendered
	virtual void onFrameStart() = 0;
	/// Called every time after a frame has been rendered
	virtual void onFrameEnd() = 0;
	/// Called once on application shutdown
	virtual void onShutdown() = 0;
};

}

#endif
