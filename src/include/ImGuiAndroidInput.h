#ifndef CLASS_NCINE_IMGUIANDROIDINPUT
#define CLASS_NCINE_IMGUIANDROIDINPUT

struct AInputEvent;

namespace ncine {

/// The class that handles Android input for ImGui
class ImGuiAndroidInput
{
  public:
	static void init();
	static void shutdown();
	static void newFrame();
	static bool processEvent(const AInputEvent *event);

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

  private:
	static bool inputEnabled_;

	/// Back and forward key events triggered by the mouse are simulated as right and middle button
	static int simulatedMouseButtonState_;
	static int mouseButtonState_;
	static unsigned int pointerCount_;

	static void updateMouseButtons();
};

}

#endif
