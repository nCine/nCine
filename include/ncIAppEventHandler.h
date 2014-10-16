#ifndef CLASS_NCIAPPEVENTHANDLER
#define CLASS_NCIAPPEVENTHANDLER

/// The interface class for handling application events (i.e. the game)
class ncIAppEventHandler
{
  public:
	virtual ~ncIAppEventHandler() { }

	/// Called once on application init
	virtual void onInit() = 0;
	/// Called every time a frame is about to be rendered
	virtual void onFrameStart() = 0;
	/// Called every time after a frame has been rendered
	virtual void onFrameEnd() = 0;
	/// Called once on application shutdown
	virtual void onShutdown() = 0;
};

#endif
