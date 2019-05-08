#ifndef CLASS_NCINE_IMGUIGLFWINPUT
#define CLASS_NCINE_IMGUIGLFWINPUT

#include "imgui.h"

struct GLFWwindow;
struct GLFWcursor;

namespace ncine {

/// The class that handles GLFW input for ImGui
class ImGuiGlfwInput
{
  public:
	static void init(GLFWwindow *window);
	static void shutdown();
	static void newFrame();

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

	static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void charCallback(GLFWwindow *window, unsigned int c);

  private:
	static bool inputEnabled_;

	static GLFWwindow *window_;
	static double time_;
	static bool mouseJustPressed_[5];
	static GLFWcursor *mouseCursors_[ImGuiMouseCursor_COUNT];

	static void updateMousePosAndButtons();
	static void updateMouseCursor();
	static void updateGamepads();
};

}

#endif
