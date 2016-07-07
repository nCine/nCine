#ifndef CLASS_NCINE_APPLICATION
#define CLASS_NCINE_APPLICATION

#include "IGfxDevice.h"
#include "AppConfiguration.h"

namespace ncine {

class FrameTimer;
class Timer;
class SceneNode;
class RenderQueue;
class ProfilePlotter;
class Font;
class TextNode;
class IInputManager;
class IAppEventHandler;

/// Main entry point and handler for nCine applications
class DLL_PUBLIC Application
{
  public:
	/// Returns the graphics device instance
	inline IGfxDevice& gfxDevice() { return *gfxDevice_; }
	/// Returns the root of the transformation graph
	inline SceneNode& rootNode() { return *rootNode_; }
	/// Returns the input manager instance
	inline IInputManager& inputManager() { return *inputManager_; }

	/// Returns the elapsed time since the end of the previous frame in milliseconds
	float interval() const;

	/// Returns the screen width
	inline int width() const { return gfxDevice_->width(); }
	/// Returns the screen height
	inline int height() const { return gfxDevice_->height(); }

	/// Returns the value of the pause flag
	inline bool isPaused() const { return isPaused_; }
	/// Sets the pause flag value
	void setPause(bool paused);
	/// Toggles the pause flag on and off
	void togglePause();

	/// Raises the quit flag
	inline void quit() { shouldQuit_ = true; }
	/// Returns the quit flag value
	inline bool shouldQuit() const { return shouldQuit_; }

	/// Returns the focus flag value
	inline bool hasFocus() const { return hasFocus_; }

  protected:
	/// Maximum length for the profile string
	static const unsigned int MaxTextLength = 256;

	bool isPaused_;
	bool hasFocus_;
	bool shouldQuit_;
	AppConfiguration appCfg_;
	FrameTimer *frameTimer_;
	IGfxDevice *gfxDevice_;
	RenderQueue *renderQueue_;
	SceneNode *rootNode_;
	Timer *profileTimer_;
	ProfilePlotter *profilePlotter_;
	Font *font_;
	TextNode *textLines_;
	float textUpdateTime_;
	String textString_;
	IInputManager *inputManager_;
	IAppEventHandler *appEventHandler_;

	Application();
	~Application() { }

	/// Must be called before giving control to the application
	void initCommon();
	/// A single step of the game loop made to render a frame
	void step();
	/// Must be called before exiting to shut down the application
	void shutdownCommon();

	/// Sets the focus flag
	virtual void setFocus(bool hasFocus);

  private:
	/// Private copy constructor
	Application(const Application&);
	/// Private assignment operator
	Application& operator=(const Application&);

	friend class PCApplication;
	friend class AndroidApplication;
};

// Meyers' Singleton
extern DLL_PUBLIC Application& theApplication();

}

#endif
