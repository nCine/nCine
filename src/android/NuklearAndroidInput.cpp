#include <cstring> // for `memcpy()`
#include <android/keycodes.h>
#include <android/input.h>

#include "NuklearContext.h"
#include "nuklear.h"
#include "NuklearAndroidInput.h"
#include "Application.h"

namespace ncine {

namespace {

	static const int MaxClipboardLength = 256;
	static char clipboard[MaxClipboardLength];

	void clipboardPaste(nk_handle usr, struct nk_text_edit *edit)
	{
		nk_textedit_paste(edit, clipboard, nk_strlen(clipboard));
	}

	void clipboardCopy(nk_handle usr, const char *text, int len)
	{
		if (len == 0)
			return;

		const int lenToCopy = (len < MaxClipboardLength) ? len : MaxClipboardLength;
		memcpy(clipboard, text, static_cast<size_t>(lenToCopy));
		clipboard[lenToCopy] = '\0';
	}

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool NuklearAndroidInput::inputEnabled_ = true;
int NuklearAndroidInput::simulatedMouseButtonState_ = 0;
int NuklearAndroidInput::mouseButtonState_ = 0;
unsigned int NuklearAndroidInput::pointerCount_ = 0;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearAndroidInput::init()
{
	NuklearContext::init();
	NuklearContext::ctx_.clip.copy = clipboardCopy;
	NuklearContext::ctx_.clip.paste = clipboardPaste;
	NuklearContext::ctx_.clip.userdata = nk_handle_ptr(nullptr);

	NuklearContext::width_ = theApplication().width();
	NuklearContext::height_ = theApplication().height();
	NuklearContext::displayWidth_ = NuklearContext::width_;
	NuklearContext::displayHeight_ = NuklearContext::height_;
}

void NuklearAndroidInput::shutdown()
{
	NuklearContext::shutdown();
}

void NuklearAndroidInput::newFrame()
{
	NuklearContext::fbScale_.x = static_cast<float>(NuklearContext::displayWidth_) / static_cast<float>(NuklearContext::width_);
	NuklearContext::fbScale_.y = static_cast<float>(NuklearContext::displayHeight_) / static_cast<float>(NuklearContext::height_);
}

void NuklearAndroidInput::inputBegin()
{
	nk_input_begin(NuklearContext::context());
}

void NuklearAndroidInput::inputEnd()
{
	nk_input_end(NuklearContext::context());
}

bool NuklearAndroidInput::processEvent(const AInputEvent *event)
{
	if (inputEnabled_ == false)
		return false;

	nk_context *ctx = NuklearContext::context();
	if ((AInputEvent_getSource(event) & AINPUT_SOURCE_KEYBOARD) == AINPUT_SOURCE_KEYBOARD &&
	    AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		int key = AKeyEvent_getKeyCode(event);
		const int down = (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN);
		const int shiftDown = ((AKeyEvent_getMetaState(event) & AMETA_SHIFT_ON) != 0);
		const int ctrlDown = ((AKeyEvent_getMetaState(event) & AMETA_CTRL_LEFT_ON) != 0);

		nk_input_key(ctx, NK_KEY_SHIFT, shiftDown);

		if (key == AKEYCODE_FORWARD_DEL)
			nk_input_key(ctx, NK_KEY_DEL, down);
		else if (key == AKEYCODE_ENTER)
			nk_input_key(ctx, NK_KEY_ENTER, down);
		else if (key == AKEYCODE_TAB)
			nk_input_key(ctx, NK_KEY_TAB, down);
		else if (key == AKEYCODE_DEL)
			nk_input_key(ctx, NK_KEY_BACKSPACE, down);
		else if (key == AKEYCODE_HOME)
		{
			nk_input_key(ctx, NK_KEY_TEXT_START, down);
			nk_input_key(ctx, NK_KEY_SCROLL_START, down);
		}
		else if (key == AKEYCODE_MOVE_END)
		{
			nk_input_key(ctx, NK_KEY_TEXT_END, down);
			nk_input_key(ctx, NK_KEY_SCROLL_END, down);
		}
		else if (key == AKEYCODE_PAGE_DOWN)
			nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
		else if (key == AKEYCODE_PAGE_UP)
			nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
		else if (key == AKEYCODE_Z)
			nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && ctrlDown);
		else if (key == AKEYCODE_R)
			nk_input_key(ctx, NK_KEY_TEXT_REDO, down && ctrlDown);
		else if (key == AKEYCODE_C)
			nk_input_key(ctx, NK_KEY_COPY, down && ctrlDown);
		else if (key == AKEYCODE_V)
			nk_input_key(ctx, NK_KEY_PASTE, down && ctrlDown);
		else if (key == AKEYCODE_X)
			nk_input_key(ctx, NK_KEY_CUT, down && ctrlDown);
		else if (key == AKEYCODE_B)
			nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && ctrlDown);
		else if (key == AKEYCODE_E)
			nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && ctrlDown);
		else if (key == AKEYCODE_DPAD_UP)
			nk_input_key(ctx, NK_KEY_UP, down);
		else if (key == AKEYCODE_DPAD_DOWN)
			nk_input_key(ctx, NK_KEY_DOWN, down);
		else if (key == AKEYCODE_DPAD_LEFT)
		{
			if (ctrlDown)
				nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
			else
				nk_input_key(ctx, NK_KEY_LEFT, down);
		}
		else if (key == AKEYCODE_DPAD_RIGHT)
		{
			if (ctrlDown)
				nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
			else
				nk_input_key(ctx, NK_KEY_RIGHT, down);
		}
		else
			return false;

		return true;
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN &&
	         AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		const int action = AMotionEvent_getAction(event);
		pointerCount_ = AMotionEvent_getPointerCount(event);

		if (action == AMOTION_EVENT_ACTION_UP)
			pointerCount_ = 0;

		updateMouseMotion(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));
		updateMouseButtons(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));

		return true;
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE &&
	         AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		const int action = AMotionEvent_getAction(event);

		// Mask out back and forward buttons in the detected state
		// as those are simulated as right and middle buttons
		int maskOutButtons = 0;
		if (simulatedMouseButtonState_ & AMOTION_EVENT_BUTTON_SECONDARY)
			maskOutButtons |= AMOTION_EVENT_BUTTON_BACK;
		if (simulatedMouseButtonState_ & AMOTION_EVENT_BUTTON_TERTIARY)
			maskOutButtons |= AMOTION_EVENT_BUTTON_FORWARD;

		switch (action)
		{
			case AMOTION_EVENT_ACTION_DOWN:
				mouseButtonState_ = AMotionEvent_getButtonState(event);
				mouseButtonState_ &= ~maskOutButtons;
				mouseButtonState_ |= simulatedMouseButtonState_;
				break;
			case AMOTION_EVENT_ACTION_UP:
				mouseButtonState_ = AMotionEvent_getButtonState(event);
				mouseButtonState_ &= ~maskOutButtons;
				mouseButtonState_ |= simulatedMouseButtonState_;
				break;
		}

		updateMouseMotion(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));
		updateMouseButtons(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));
		nk_input_scroll(ctx, nk_vec2(AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HSCROLL, 0), AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_VSCROLL, 0)));

		return true;
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE &&
	         AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		const int keyCode = AKeyEvent_getKeyCode(event);
		if (keyCode == AKEYCODE_BACK || keyCode == AKEYCODE_FORWARD)
		{
			const int simulatedButton = (keyCode == AKEYCODE_BACK) ? AMOTION_EVENT_BUTTON_SECONDARY : AMOTION_EVENT_BUTTON_TERTIARY;
			static int oldAction = AKEY_EVENT_ACTION_UP;
			const int action = AKeyEvent_getAction(event);

			// checking previous action to avoid key repeat events
			if (action == AKEY_EVENT_ACTION_DOWN && oldAction == AKEY_EVENT_ACTION_UP)
			{
				oldAction = action;
				simulatedMouseButtonState_ |= simulatedButton;
				mouseButtonState_ |= simulatedButton;
			}
			else if (action == AKEY_EVENT_ACTION_UP && oldAction == AKEY_EVENT_ACTION_DOWN)
			{
				oldAction = action;
				simulatedMouseButtonState_ &= ~simulatedButton;
				mouseButtonState_ &= ~simulatedButton;
			}

			updateMouseButtons(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));
		}
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearAndroidInput::updateMouseMotion(float x, float y)
{
	const int scaledX = static_cast<int>(x / NuklearContext::fbScale_.x);
	const int scaledY = static_cast<int>(y / NuklearContext::fbScale_.y);

	nk_input_motion(NuklearContext::context(), scaledX, scaledY);
}

void NuklearAndroidInput::updateMouseButtons(float x, float y)
{
	const int scaledX = static_cast<int>(x / NuklearContext::fbScale_.x);
	const int scaledY = static_cast<int>(y / NuklearContext::fbScale_.y);

	if (pointerCount_ > 0)
	{
		nk_input_button(NuklearContext::context(), NK_BUTTON_LEFT, scaledX, scaledY, (pointerCount_ == 1));
		nk_input_button(NuklearContext::context(), NK_BUTTON_RIGHT, scaledX, scaledY, (pointerCount_ == 2));
	}
	else
	{
		nk_input_button(NuklearContext::context(), NK_BUTTON_LEFT, scaledX, scaledY, (mouseButtonState_ & AMOTION_EVENT_BUTTON_PRIMARY));
		nk_input_button(NuklearContext::context(), NK_BUTTON_RIGHT, scaledX, scaledY, (mouseButtonState_ & AMOTION_EVENT_BUTTON_SECONDARY));
		nk_input_button(NuklearContext::context(), NK_BUTTON_MIDDLE, scaledX, scaledY, (mouseButtonState_ & AMOTION_EVENT_BUTTON_TERTIARY));
	}
}

}
