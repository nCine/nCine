#ifndef CLASS_NCINE_NUKLEARANDROIDINPUT
#define CLASS_NCINE_NUKLEARANDROIDINPUT

struct AInputEvent;

namespace ncine {

/// The class that handles Android input for Nuklear
class NuklearAndroidInput
{
  public:
	static void init();
	static void shutdown();
	static void newFrame();

	static void inputBegin();
	static void inputEnd();
	static bool processEvent(const AInputEvent *event);

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

  private:
	static bool inputEnabled_;

	/// Back and forward key events triggered by the mouse are simulated as right and middle button
	static int simulatedMouseButtonState_;
	static int mouseButtonState_;
	static unsigned int pointerCount_;

	static void updateMouseMotion(float x, float y);
	static void updateMouseButtons(float x, float y);
};

}

#endif
