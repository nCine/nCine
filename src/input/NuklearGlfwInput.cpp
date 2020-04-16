// Based on demo/glfw_opengl3/nuklear_glfw_gl3.h

#include <cstring> // for `memcpy()`
#include <GLFW/glfw3.h>
#include "NuklearGlfwInput.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const double NuklearGlfwInput::NK_GLFW_DOUBLE_CLICK_LO = 0.02;
const double NuklearGlfwInput::NK_GLFW_DOUBLE_CLICK_HI = 0.2;

bool NuklearGlfwInput::inputEnabled_ = true;
GLFWwindow *NuklearGlfwInput::window_ = nullptr;

unsigned int NuklearGlfwInput::text_[NK_GLFW_TEXT_MAX];
int NuklearGlfwInput::textLength_;
struct nk_vec2 NuklearGlfwInput::scroll_;
double NuklearGlfwInput::lastButtonClick_;
int NuklearGlfwInput::isDoubleClickDown_;
struct nk_vec2 NuklearGlfwInput::doubleClickPos_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearGlfwInput::init(GLFWwindow *window)
{
	window_ = window;

	NuklearContext::init();
	NuklearContext::ctx_.clip.copy = clipboardCopy;
	NuklearContext::ctx_.clip.paste = clipboardPaste;
	NuklearContext::ctx_.clip.userdata = nk_handle_ptr(nullptr);
	lastButtonClick_ = 0.0;
	isDoubleClickDown_ = nk_false;
	doubleClickPos_ = nk_vec2(0.0f, 0.0f);
}

void NuklearGlfwInput::shutdown()
{
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

	nk_input_key(ctx, NK_KEY_DEL, glfwGetKey(window_, GLFW_KEY_DELETE) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_ENTER, glfwGetKey(window_, GLFW_KEY_ENTER) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_TAB, glfwGetKey(window_, GLFW_KEY_TAB) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_BACKSPACE, glfwGetKey(window_, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_UP, glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_DOWN, glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_TEXT_START, glfwGetKey(window_, GLFW_KEY_HOME) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_TEXT_END, glfwGetKey(window_, GLFW_KEY_END) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_SCROLL_START, glfwGetKey(window_, GLFW_KEY_HOME) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_SCROLL_END, glfwGetKey(window_, GLFW_KEY_END) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_SCROLL_DOWN, glfwGetKey(window_, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_SCROLL_UP, glfwGetKey(window_, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
	nk_input_key(ctx, NK_KEY_SHIFT, glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window_, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

	if (glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window_, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
	{
		nk_input_key(ctx, NK_KEY_COPY, glfwGetKey(window_, GLFW_KEY_C) == GLFW_PRESS);
		nk_input_key(ctx, NK_KEY_PASTE, glfwGetKey(window_, GLFW_KEY_V) == GLFW_PRESS);
		nk_input_key(ctx, NK_KEY_CUT, glfwGetKey(window_, GLFW_KEY_X) == GLFW_PRESS);
		nk_input_key(ctx, NK_KEY_TEXT_UNDO, glfwGetKey(window_, GLFW_KEY_Z) == GLFW_PRESS);
		nk_input_key(ctx, NK_KEY_TEXT_REDO, glfwGetKey(window_, GLFW_KEY_R) == GLFW_PRESS);
		nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS);
		nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS);
		nk_input_key(ctx, NK_KEY_TEXT_LINE_START, glfwGetKey(window_, GLFW_KEY_B) == GLFW_PRESS);
		nk_input_key(ctx, NK_KEY_TEXT_LINE_END, glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS);
	}
	else
	{
		nk_input_key(ctx, NK_KEY_LEFT, glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS);
		nk_input_key(ctx, NK_KEY_RIGHT, glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS);
		nk_input_key(ctx, NK_KEY_COPY, 0);
		nk_input_key(ctx, NK_KEY_PASTE, 0);
		nk_input_key(ctx, NK_KEY_CUT, 0);
		nk_input_key(ctx, NK_KEY_SHIFT, 0);
	}

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

void NuklearGlfwInput::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
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
	if (inputEnabled_ == false)
		return;

	scroll_.x += static_cast<float>(xoffset);
	scroll_.y += static_cast<float>(yoffset);
}

void NuklearGlfwInput::charCallback(GLFWwindow *window, unsigned int c)
{
	if (inputEnabled_ == false)
		return;

	if (textLength_ < NK_GLFW_TEXT_MAX)
		text_[textLength_++] = c;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

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

	nctl::UniquePtr<char[]> string = nctl::makeUnique<char[]>(len + 1);
	if (string == nullptr)
		return;
	memcpy(string.get(), text, static_cast<size_t>(len));
	string[len] = '\0';
	glfwSetClipboardString(window_, string.get());
}

}
