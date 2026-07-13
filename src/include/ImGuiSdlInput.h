#ifndef CLASS_NCINE_IMGUISDLINPUT
#define CLASS_NCINE_IMGUISDLINPUT

#include "imgui.h"

struct SDL_Window;
union SDL_Event;
struct SDL_Cursor;
#ifdef WITH_SDL2
struct SDL_Renderer;
typedef struct _SDL_GameController SDL_GameController;
#else
typedef struct SDL_Gamepad SDL_Gamepad;
#endif

namespace ncine {

/// The class that handles SDL input for ImGui
class ImGuiSdlInput
{
  public:
	static void init(SDL_Window *window);
	static void shutdown();
	static void newFrame();
	static bool processEvent(const SDL_Event *event);

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

  private:
	enum class GamepadMode
	{
		AUTO_FIRST,
		AUTO_ALL,
		MANUAL
	};

	enum class MouseCaptureMode
	{
		ENABLED,
		ENABLED_AFTER_DRAG,
		DISABLED
	};

	static bool inputEnabled_;

	static SDL_Window *window_;
	static unsigned long int time_;
	static char *clipboardTextData_;

#ifdef WITH_SDL3
	// Text input handling
	static SDL_Window *imeWindow_;
	static bool imeDirty_;
#endif

	// Mouse handling
	static unsigned int mouseWindowID_;
	static int mouseButtonsDown_;
	static SDL_Cursor *mouseCursors_[ImGuiMouseCursor_COUNT];
	static SDL_Cursor *mouseLastCursor_;
#ifdef WITH_SDL2
	static unsigned int mouseLastLeaveFrame_;
#else
	static unsigned int mousePendingLeaveFrame_;
#endif
	static bool mouseCanUseGlobalState_;
	static MouseCaptureMode mouseCaptureMode_;

	// Gamepad handling
#ifdef WITH_SDL2
	static ImVector<SDL_GameController *> gamepads_;
#else
	static ImVector<SDL_Gamepad *> gamepads_;
#endif
	static GamepadMode gamepadMode_;
	static bool wantUpdateGamepadsList_;

	static const char *clipboardText(ImGuiContext *context);
#ifdef WITH_SDL3
	static void updateIme();
#endif
	static void updateMouseData();
	static void updateMouseCursor();
	static float getContentScaleForWindow(SDL_Window *window);
	static float getContentScaleForDisplay(int displayIndex);
	static void closeGamepads();
#ifdef WITH_SDL2
	static void setGamepadMode(GamepadMode mode, SDL_GameController **manualGamepadsArray, unsigned int manualGamepadsCount);
#else
	static void setGamepadMode(GamepadMode mode, SDL_Gamepad **manualGamepadsArray, unsigned int manualGamepadsCount);
#endif
	static void updateGamepads();
	static void setMouseCaptureMode(MouseCaptureMode mode);
#ifdef WITH_SDL2
	static void getWindowSizeAndFramebufferScale(SDL_Window *window, SDL_Renderer *renderer, ImVec2 *outSize, ImVec2 *outFramebufferScale);
#else
	static void getWindowSizeAndFramebufferScale(SDL_Window *window, ImVec2 *outSize, ImVec2 *outFramebufferScale);
#endif
};

}

#endif
