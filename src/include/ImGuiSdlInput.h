#ifndef CLASS_NCINE_IMGUISDLINPUT
#define CLASS_NCINE_IMGUISDLINPUT

#include "imgui.h"

struct SDL_Window;
union SDL_Event;
struct SDL_Cursor;

namespace ncine {

/// The class that handles SDL2 input for ImGui
class ImGuiSdlInput
{
  public:
	static void init(SDL_Window *window);
	static void shutdown();
	static void newFrame();
	static bool processEvent(const SDL_Event *event);

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

  private:
	static bool inputEnabled_;
	static bool mouseCanUseGlobalState_;
	static unsigned int pendingMouseLeaveFrame_;

	static SDL_Window *window_;
	static unsigned long int time_;
	static unsigned int mouseWindowID_;
	static int mouseButtonsDown_;
	static SDL_Cursor *mouseCursors_[ImGuiMouseCursor_COUNT];
	static SDL_Cursor *lastMouseCursor_;
	static char *clipboardTextData_;

	static const char *clipboardText(void *userData);
	static void updateMouseData();
	static void updateMouseCursor();
	static void updateGamepads();
};

}

#endif
