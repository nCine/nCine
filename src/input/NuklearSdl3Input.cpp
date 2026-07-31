// Based on demo/sdl_opengl3/nuklear_sdl_gl3.h

#include <SDL3/SDL.h>
#include "NuklearSdlInput.h"
#include "SdlInputManager.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool NuklearSdlInput::inputEnabled_ = true;
SDL_Window *NuklearSdlInput::window_ = nullptr;
bool NuklearSdlInput::editWasActive_ = false;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearSdlInput::init(SDL_Window *window)
{
	window_ = window;

	NuklearContext::init();
	NuklearContext::ctx_.clip.copy = clipboardCopy;
	NuklearContext::ctx_.clip.paste = clipboardPaste;
	NuklearContext::ctx_.clip.userdata = nk_handle_ptr(nullptr);
}

void NuklearSdlInput::shutdown()
{
	NuklearContext::shutdown();
	window_ = nullptr;
}

void NuklearSdlInput::newFrame()
{
	SDL_GetWindowSize(window_, &NuklearContext::width_, &NuklearContext::height_);
	SDL_GetWindowSizeInPixels(window_, &NuklearContext::displayWidth_, &NuklearContext::displayHeight_);

	NuklearContext::fbScale_.x = static_cast<float>(NuklearContext::displayWidth_) / static_cast<float>(NuklearContext::width_);
	NuklearContext::fbScale_.y = static_cast<float>(NuklearContext::displayHeight_) / static_cast<float>(NuklearContext::height_);
}

void NuklearSdlInput::inputBegin()
{
	updateTextInput();
	nk_input_begin(NuklearContext::context());
}

void NuklearSdlInput::inputEnd()
{
	nk_input_end(NuklearContext::context());
}

bool NuklearSdlInput::processEvent(const SDL_Event *event)
{
	if (inputEnabled_ == false)
		return false;

	nk_context *ctx = NuklearContext::context();
	if (event->type == SDL_EVENT_KEY_UP || event->type == SDL_EVENT_KEY_DOWN)
	{
		/* key events */
		const int down = (event->type == SDL_EVENT_KEY_DOWN);
		const bool *state = SDL_GetKeyboardState(nullptr);
		const SDL_Keycode sym = event->key.key;
		if (sym == SDLK_RSHIFT || sym == SDLK_LSHIFT)
			nk_input_key(ctx, NK_KEY_SHIFT, down);
		else if (sym == SDLK_DELETE)
			nk_input_key(ctx, NK_KEY_DEL, down);
		else if (sym == SDLK_RETURN)
			nk_input_key(ctx, NK_KEY_ENTER, down);
		else if (sym == SDLK_TAB)
			nk_input_key(ctx, NK_KEY_TAB, down);
		else if (sym == SDLK_BACKSPACE)
			nk_input_key(ctx, NK_KEY_BACKSPACE, down);
		else if (sym == SDLK_HOME)
		{
			nk_input_key(ctx, NK_KEY_TEXT_START, down);
			nk_input_key(ctx, NK_KEY_SCROLL_START, down);
		}
		else if (sym == SDLK_END)
		{
			nk_input_key(ctx, NK_KEY_TEXT_END, down);
			nk_input_key(ctx, NK_KEY_SCROLL_END, down);
		}
		else if (sym == SDLK_PAGEDOWN)
			nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
		else if (sym == SDLK_PAGEUP)
			nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
		else if (sym == SDLK_Z)
			nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_R)
			nk_input_key(ctx, NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_C)
			nk_input_key(ctx, NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_V)
			nk_input_key(ctx, NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_X)
			nk_input_key(ctx, NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_B)
			nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_E)
			nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_UP)
			nk_input_key(ctx, NK_KEY_UP, down);
		else if (sym == SDLK_DOWN)
			nk_input_key(ctx, NK_KEY_DOWN, down);
		else if (sym == SDLK_LEFT)
		{
			if (state[SDL_SCANCODE_LCTRL])
				nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
			else
				nk_input_key(ctx, NK_KEY_LEFT, down);
		}
		else if (sym == SDLK_RIGHT)
		{
			if (state[SDL_SCANCODE_LCTRL])
				nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
			else
				nk_input_key(ctx, NK_KEY_RIGHT, down);
		}
		else
			return false;
		return true;
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP)
	{
		/* mouse button */
		const int down = (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN);
		const int x = static_cast<int>(event->button.x);
		const int y = static_cast<int>(event->button.y);
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			if (event->button.clicks > 1)
				nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
			nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
		}
		else if (event->button.button == SDL_BUTTON_MIDDLE)
			nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
		else if (event->button.button == SDL_BUTTON_RIGHT)
			nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
		return true;
	}
	else if (event->type == SDL_EVENT_MOUSE_MOTION)
	{
		/* mouse motion */
		if (ctx->input.mouse.grabbed)
		{
			const int x = static_cast<int>(ctx->input.mouse.prev.x);
			const int y = static_cast<int>(ctx->input.mouse.prev.y);
			nk_input_motion(ctx, x + event->motion.xrel, y + event->motion.yrel);
		}
		else
			nk_input_motion(ctx, event->motion.x, event->motion.y);
		return true;
	}
	else if (event->type == SDL_EVENT_TEXT_INPUT)
	{
		/* text input */
		nk_glyph glyph;
		memcpy(glyph, event->text.text, NK_UTF_SIZE);
		nk_input_glyph(ctx, glyph);
		return true;
	}
	else if (event->type == SDL_EVENT_MOUSE_WHEEL)
	{
		/* mouse wheel */
		nk_input_scroll(ctx, nk_vec2(static_cast<float>(event->wheel.x), static_cast<float>(event->wheel.y)));
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearSdlInput::clipboardPaste(nk_handle usr, struct nk_text_edit *edit)
{
	const char *text = SDL_GetClipboardText();
	if (text)
		nk_textedit_paste(edit, text, nk_strlen(text));
}

void NuklearSdlInput::clipboardCopy(nk_handle usr, const char *text, int len)
{
	if (len == 0)
		return;
	SDL_SetClipboardText(text);
}

void NuklearSdlInput::updateTextInput()
{
	nk_context *ctx = NuklearContext::context();

	bool active = false;

	/* Determine if Nuklear is using any top-level "edit" widget.
	 * Popups take higher priority because they block any incomming input.
	 * This will not work, if the widget is not updating context state properly. */
	if (!ctx->active)
		active = false;
	else if (ctx->active->popup.win)
		active = ctx->active->popup.win->edit.active;
	else
		active = ctx->active->edit.active;

	/* ImGui independently wants to do the same for its own active widget, so the
	 * actual Start/Stop calls go through a reference count instead of reading
	 * back `SDL_TextInputActive()`, which does not tell them apart. */
	if (active != editWasActive_)
	{
		if (active)
			SdlInputManager::acquireTextInput(window_);
		else
			SdlInputManager::releaseTextInput(window_);
		editWasActive_ = active;
	}

	/* FIXME:
	 * for full SDL3 integration, you also need to find current edit widget
	 * bounds and the text cursor offset, and pass this data into SDL_SetTextInputArea.
	 * This is currently not possible to do safely as Nuklear does not support it.
	 * https://wiki.libsdl.org/SDL3/SDL_SetTextInputArea
	 * https://github.com/Immediate-Mode-UI/Nuklear/pull/857
	 */
}

}
