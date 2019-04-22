#ifndef CLASS_NCINE_NUKLEARGLFWINPUT
#define CLASS_NCINE_NUKLEARGLFWINPUT

#include "NuklearContext.h"
#include "nuklear.h"

struct GLFWwindow;
struct GLFWcursor;

namespace ncine {

/// The class that handles GLFW input for Nuklear
class NuklearGlfwInput
{
  public:
	static void init(GLFWwindow *window);
	static void shutdown();
	static void newFrame();

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

	static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	static void charCallback(GLFWwindow *window, unsigned int c);

  private:
	static const double NK_GLFW_DOUBLE_CLICK_LO;
	static const double NK_GLFW_DOUBLE_CLICK_HI;

	static bool inputEnabled_;
	static GLFWwindow *window_;

	static const int NK_GLFW_TEXT_MAX = 256;
	static unsigned int text_[NK_GLFW_TEXT_MAX];
	static int textLength_;
	static struct nk_vec2 scroll_;
	static double lastButtonClick_;
	static int isDoubleClickDown_;
	static struct nk_vec2 doubleClickPos_;

	static void clipboardPaste(nk_handle usr, struct nk_text_edit *edit);
	static void clipboardCopy(nk_handle usr, const char *text_, int len);
};

}

#endif
