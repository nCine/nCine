#ifndef CLASS_NCIAPPEVENTHANDLER
#define CLASS_NCIAPPEVENTHANDLER

/// The interface class for handling application events (i.e. the game)
class ncIAppEventHandler
{
public:
	/// Called once on application init
	virtual void OnInit() = 0;
	/// Called every time a frame is about to be rendered
	virtual void OnFrameStart() = 0;
	/// Called every time after a frame has been rendered
	virtual void OnFrameEnd() = 0;
	/// Called once on application shutdown
	virtual void OnShutdown() = 0;
};

#endif
