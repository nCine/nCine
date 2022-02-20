// Based on demo/glfw_opengl3/nuklear_glfw_gl3.h

#include <GLFW/glfw3.h>
#include "NuklearGlfwInput.h"

namespace ncine {

namespace {

	GLFWmousebuttonfun prevUserCallbackMousebutton = nullptr;
	GLFWscrollfun prevUserCallbackScroll = nullptr;
	GLFWcharfun prevUserCallbackChar = nullptr;

	const int numKeys = 12;
	const int nuklearGlfwKeys[numKeys * 2] = {
		NK_KEY_DEL, GLFW_KEY_DELETE,
		NK_KEY_ENTER, GLFW_KEY_ENTER,
		NK_KEY_TAB, GLFW_KEY_TAB,
		NK_KEY_BACKSPACE, GLFW_KEY_BACKSPACE,
		NK_KEY_UP, GLFW_KEY_UP,
		NK_KEY_DOWN, GLFW_KEY_DOWN,
		NK_KEY_TEXT_START, GLFW_KEY_HOME,
		NK_KEY_TEXT_END, GLFW_KEY_END,
		NK_KEY_SCROLL_START, GLFW_KEY_HOME,
		NK_KEY_SCROLL_END, GLFW_KEY_END,
		NK_KEY_SCROLL_DOWN, GLFW_KEY_PAGE_DOWN,
		NK_KEY_SCROLL_UP, GLFW_KEY_PAGE_UP
	};

	const int numCtrlKeys = 9;
	const int nuklearGlfwCtrlKeys[numCtrlKeys * 2] = {
		NK_KEY_COPY, GLFW_KEY_C,
		NK_KEY_PASTE, GLFW_KEY_V,
		NK_KEY_CUT, GLFW_KEY_X,
		NK_KEY_TEXT_UNDO, GLFW_KEY_Z,
		NK_KEY_TEXT_REDO, GLFW_KEY_R,
		NK_KEY_TEXT_WORD_LEFT, GLFW_KEY_LEFT,
		NK_KEY_TEXT_WORD_RIGHT, GLFW_KEY_RIGHT,
		NK_KEY_TEXT_LINE_START, GLFW_KEY_B,
		NK_KEY_TEXT_LINE_END, GLFW_KEY_E
	};

	const int numNonCtrlKeys = 2;
	const int nuklearGlfwNonCtrlKeys[numNonCtrlKeys * 2] = {
		NK_KEY_TEXT_WORD_LEFT, GLFW_KEY_LEFT,
		NK_KEY_TEXT_WORD_RIGHT, GLFW_KEY_RIGHT
	};

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const double NuklearGlfwInput::NK_GLFW_DOUBLE_CLICK_LO = 0.02;
const double NuklearGlfwInput::NK_GLFW_DOUBLE_CLICK_HI = 0.2;

bool NuklearGlfwInput::inputEnabled_ = true;
GLFWwindow *NuklearGlfwInput::window_ = nullptr;

unsigned int NuklearGlfwInput::text_[NK_GLFW_TEXT_MAX];
int NuklearGlfwInput::textLength_;
char NuklearGlfwInput::keys_[NK_KEY_MAX];
struct nk_vec2 NuklearGlfwInput::scroll_;
double NuklearGlfwInput::lastButtonClick_;
int NuklearGlfwInput::isDoubleClickDown_;
struct nk_vec2 NuklearGlfwInput::doubleClickPos_;

bool NuklearGlfwInput::installedCallbacks_ = false;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearGlfwInput::init(GLFWwindow *window, bool withCallbacks)
{
	window_ = window;

	NuklearContext::init();
	NuklearContext::ctx_.clip.copy = clipboardCopy;
	NuklearContext::ctx_.clip.paste = clipboardPaste;
	NuklearContext::ctx_.clip.userdata = nk_handle_ptr(nullptr);

	for (unsigned int i = 0; i < NK_KEY_MAX; i++)
		keys_[i] = 0;
	lastButtonClick_ = 0.0;
	isDoubleClickDown_ = nk_false;
	doubleClickPos_ = nk_vec2(0.0f, 0.0f);

	if (withCallbacks)
		installCallbacks(window);
}

void NuklearGlfwInput::shutdown()
{
	if (installedCallbacks_)
		restoreCallbacks(window_);

	NuklearContext::shutdown();
	window_ = nullptr;
}

void NuklearGlfwInput::newFrame()
{
	int i;
	double x, y;

	glfwGetWindowSize(window_, &NuklearContext::width_, &NuklearContext::height_);
	glfwGetFramebufferSize(window_, &NuklearContext::displayWidth_, &NuklearContext::displayHeight_);
	NuklearContext::fbScale_.x = static_cast<float>(NuklearContext::displayWidth_) / static_cast<float>(NuklearContext::width_);
	NuklearContext::fbScale_.y = static_cast<float>(NuklearContext::displayHeight_) / static_cast<float>(NuklearContext::height_);

	nk_context *ctx = &NuklearContext::ctx_;

	nk_input_begin(ctx);
	for (i = 0; i < textLength_; ++i)
		nk_input_unicode(ctx, text_[i]);

	/* optional grabbing behavior */
	if (ctx->input.mouse.grab)
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	else if (ctx->input.mouse.ungrab)
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Update keys (without repeating)
	updateKeys(nuklearGlfwKeys, numKeys);

	const bool ctrlIsPressed = glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window_, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
	nk_input_key(ctx, NK_KEY_CTRL, ctrlIsPressed);

	if (ctrlIsPressed)
		updateKeys(nuklearGlfwCtrlKeys, numCtrlKeys);
	else
		updateKeys(nuklearGlfwNonCtrlKeys, numNonCtrlKeys);

	glfwGetCursorPos(window_, &x, &y);
	nk_input_motion(ctx, static_cast<int>(x), static_cast<int>(y));

	if (ctx->input.mouse.grabbed)
	{
		glfwSetCursorPos(window_, ctx->input.mouse.prev.x, ctx->input.mouse.prev.y);
		ctx->input.mouse.pos.x = ctx->input.mouse.prev.x;
		ctx->input.mouse.pos.y = ctx->input.mouse.prev.y;
	}

	nk_input_button(ctx, NK_BUTTON_LEFT, static_cast<int>(x), static_cast<int>(y), glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
	nk_input_button(ctx, NK_BUTTON_MIDDLE, static_cast<int>(x), static_cast<int>(y), glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
	nk_input_button(ctx, NK_BUTTON_RIGHT, static_cast<int>(x), static_cast<int>(y), glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
	nk_input_button(ctx, NK_BUTTON_DOUBLE, static_cast<int>(doubleClickPos_.x), static_cast<int>(doubleClickPos_.y), isDoubleClickDown_);
	nk_input_scroll(ctx, scroll_);
	nk_input_end(ctx);
	textLength_ = 0;
	scroll_ = nk_vec2(0.0f, 0.0f);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearGlfwInput::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (prevUserCallbackMousebutton != nullptr && window == window_)
		prevUserCallbackMousebutton(window, button, action, mods);

	if (inputEnabled_ == false)
		return;

	double x, y;
	if (button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	glfwGetCursorPos(window, &x, &y);
	if (action == GLFW_PRESS)
	{
		const double delta = glfwGetTime() - lastButtonClick_;
		if (delta > NK_GLFW_DOUBLE_CLICK_LO && delta < NK_GLFW_DOUBLE_CLICK_HI)
		{
			isDoubleClickDown_ = nk_true;
			doubleClickPos_ = nk_vec2(static_cast<float>(x), static_cast<float>(y));
		}
		lastButtonClick_ = glfwGetTime();
	}
	else
		isDoubleClickDown_ = nk_false;
}

void NuklearGlfwInput::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	if (prevUserCallbackScroll != nullptr && window == window_)
		prevUserCallbackScroll(window, xoffset, yoffset);

	if (inputEnabled_ == false)
		return;

	scroll_.x += static_cast<float>(xoffset);
	scroll_.y += static_cast<float>(yoffset);
}

void NuklearGlfwInput::charCallback(GLFWwindow *window, unsigned int c)
{
	if (prevUserCallbackChar != nullptr && window == window_)
		prevUserCallbackChar(window, c);

	if (inputEnabled_ == false)
		return;

	if (textLength_ < NK_GLFW_TEXT_MAX)
		text_[textLength_++] = c;
	text_[textLength_] = '\0';
}

void NuklearGlfwInput::installCallbacks(GLFWwindow *window)
{
	ASSERT_MSG(installedCallbacks_ == false, "Callbacks already installed!");
	ASSERT(window_ == window);

	prevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, mouseButtonCallback);
	prevUserCallbackScroll = glfwSetScrollCallback(window, scrollCallback);
	prevUserCallbackChar = glfwSetCharCallback(window, charCallback);
	installedCallbacks_ = true;
}

void NuklearGlfwInput::restoreCallbacks(GLFWwindow *window)
{
	ASSERT_MSG(installedCallbacks_ == true, "Callbacks not installed!");
	ASSERT(window_ == window);

	glfwSetMouseButtonCallback(window, prevUserCallbackMousebutton);
	glfwSetScrollCallback(window, prevUserCallbackScroll);
	glfwSetCharCallback(window, prevUserCallbackChar);

	installedCallbacks_ = false;
	prevUserCallbackMousebutton = nullptr;
	prevUserCallbackScroll = nullptr;
	prevUserCallbackChar = nullptr;
}

void NuklearGlfwInput::clipboardPaste(nk_handle usr, struct nk_text_edit *edit)
{
	const char *text = glfwGetClipboardString(window_);
	if (text)
		nk_textedit_paste(edit, text, nk_strlen(text));
}

void NuklearGlfwInput::clipboardCopy(nk_handle usr, const char *text, int len)
{
	if (len == 0)
		return;
	glfwSetClipboardString(window_, text);
}

void NuklearGlfwInput::updateKeys(const int keyMap[], unsigned int numKeys)
{
	nk_context *ctx = &NuklearContext::ctx_;
	for (unsigned int i = 0; i < numKeys; i += 2)
	{
		const int nuklearKey = keyMap[i];
		const int glfwKey = keyMap[i + 1];
		const bool keyIsPressed = glfwGetKey(window_, glfwKey) == GLFW_PRESS;
		if (keys_[nuklearKey] != keyIsPressed)
		{
			nk_input_key(ctx, static_cast<nk_keys>(nuklearKey), keyIsPressed);
			keys_[nuklearKey] = keyIsPressed;
		}
	}
}

}
