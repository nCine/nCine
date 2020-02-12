// Based on demo/sdl_opengl3/nuklear_sdl_gl3.h

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "NuklearSdlInput.h"

namespace ncine {

namespace {

	void *nuklearAlloc(nk_handle usr, void *old, nk_size size)
	{
		unsigned char *ptr = new unsigned char[size];
		return reinterpret_cast<void *>(ptr);
	}

	void nuklearFree(nk_handle usr, void *old)
	{
		unsigned char *ptr = reinterpret_cast<unsigned char *>(old);
		delete[] ptr;
	}

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool NuklearSdlInput::inputEnabled_ = true;
SDL_Window *NuklearSdlInput::window_ = nullptr;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearSdlInput::init(SDL_Window *window)
{
	window_ = window;

	nk_allocator allocator{ { nullptr }, nuklearAlloc, nuklearFree };
	nk_init(&NuklearContext::ctx_, &allocator, nullptr);
	NuklearContext::ctx_.clip.copy = clipboardCopy;
	NuklearContext::ctx_.clip.paste = clipboardPaste;
	NuklearContext::ctx_.clip.userdata = nk_handle_ptr(nullptr);
}

void NuklearSdlInput::shutdown()
{
	nk_font_atlas_clear(&NuklearContext::atlas_);
	nk_free(&NuklearContext::ctx_);
	nk_buffer_free(&NuklearContext::cmds_);

	window_ = nullptr;
}

void NuklearSdlInput::newFrame()
{
	SDL_GetWindowSize(window_, &NuklearContext::width_, &NuklearContext::height_);
	SDL_GL_GetDrawableSize(window_, &NuklearContext::displayWidth_, &NuklearContext::displayHeight_);

	NuklearContext::fbScale_.x = static_cast<float>(NuklearContext::displayWidth_) / static_cast<float>(NuklearContext::width_);
	NuklearContext::fbScale_.y = static_cast<float>(NuklearContext::displayHeight_) / static_cast<float>(NuklearContext::height_);
}

void NuklearSdlInput::inputBegin()
{
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
	if (event->type == SDL_KEYUP || event->type == SDL_KEYDOWN)
	{
		/* key events */
		const int down = (event->type == SDL_KEYDOWN);
		const Uint8 *state = SDL_GetKeyboardState(nullptr);
		const SDL_Keycode sym = event->key.keysym.sym;
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
		else if (sym == SDLK_z)
			nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_r)
			nk_input_key(ctx, NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_c)
			nk_input_key(ctx, NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_v)
			nk_input_key(ctx, NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_x)
			nk_input_key(ctx, NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_b)
			nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]);
		else if (sym == SDLK_e)
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
	else if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP)
	{
		/* mouse button */
		const int down = (event->type == SDL_MOUSEBUTTONDOWN);
		const int x = event->button.x, y = event->button.y;
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
	else if (event->type == SDL_MOUSEMOTION)
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
	else if (event->type == SDL_TEXTINPUT)
	{
		/* text input */
		nk_glyph glyph;
		memcpy(glyph, event->text.text, NK_UTF_SIZE);
		nk_input_glyph(ctx, glyph);
		return true;
	}
	else if (event->type == SDL_MOUSEWHEEL)
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

	nctl::UniquePtr<char[]> string = nctl::makeUnique<char[]>(len + 1);
	if (string == nullptr)
		return;
	memcpy(string.get(), text, static_cast<size_t>(len));
	string[len] = '\0';
	SDL_SetClipboardText(string.get());
}

}
