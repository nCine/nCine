#ifndef CLASS_NCIAPPEVENTHANDLER
#define CLASS_NCIAPPEVENTHANDLER

/// The interface class for handling application events (i.e. the game)
class ncIAppEventHandler
{
public:
	virtual void OnInit() = 0;
	virtual void OnFrameStart() = 0;
	virtual void OnFrameEnd() = 0;
	virtual void OnShutdown() = 0;
};

#endif
