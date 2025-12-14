#include "SdlInputManager.h"
#include <SDL2/SDL_keycode.h>

namespace ncine {

KeySym SdlKeys::keySymValueToEnum(int keysym)
{
	// clang-format off
	switch (keysym)
	{
		case SDLK_UNKNOWN:			return KeySym::UNKNOWN;
		case SDLK_BACKSPACE:		return KeySym::BACKSPACE;
		case SDLK_TAB:				return KeySym::TAB;
		case SDLK_CLEAR:			return KeySym::CLEAR;
		case SDLK_RETURN:			return KeySym::RETURN;
		case SDLK_PAUSE:			return KeySym::PAUSE;
		case SDLK_ESCAPE:			return KeySym::ESCAPE;
		case SDLK_SPACE:			return KeySym::SPACE;
		case SDLK_EXCLAIM:			return KeySym::EXCLAIM;
		case SDLK_QUOTEDBL:			return KeySym::QUOTEDBL;
		case SDLK_HASH:				return KeySym::HASH;
		case SDLK_DOLLAR:			return KeySym::DOLLAR;
		case SDLK_AMPERSAND:		return KeySym::AMPERSAND;
		case SDLK_QUOTE:			return KeySym::QUOTE;
		case SDLK_LEFTPAREN:		return KeySym::LEFTPAREN;
		case SDLK_RIGHTPAREN:		return KeySym::RIGHTPAREN;
		case SDLK_ASTERISK:			return KeySym::ASTERISK;
		case SDLK_PLUS:				return KeySym::PLUS;
		case SDLK_COMMA:			return KeySym::COMMA;
		case SDLK_MINUS:			return KeySym::MINUS;
		case SDLK_PERIOD:			return KeySym::PERIOD;
		case SDLK_SLASH:			return KeySym::SLASH;
		case SDLK_0:				return KeySym::N0;
		case SDLK_1:				return KeySym::N1;
		case SDLK_2:				return KeySym::N2;
		case SDLK_3:				return KeySym::N3;
		case SDLK_4:				return KeySym::N4;
		case SDLK_5:				return KeySym::N5;
		case SDLK_6:				return KeySym::N6;
		case SDLK_7:				return KeySym::N7;
		case SDLK_8:				return KeySym::N8;
		case SDLK_9:				return KeySym::N9;
		case SDLK_COLON:			return KeySym::COLON;
		case SDLK_SEMICOLON:		return KeySym::SEMICOLON;
		case SDLK_LESS:				return KeySym::LESS;
		case SDLK_EQUALS:			return KeySym::EQUALS;
		case SDLK_GREATER:			return KeySym::GREATER;
		case SDLK_QUESTION:			return KeySym::QUESTION;
		case SDLK_AT:				return KeySym::AT;

		case SDLK_LEFTBRACKET:		return KeySym::LEFTBRACKET;
		case SDLK_BACKSLASH:		return KeySym::BACKSLASH;
		case SDLK_RIGHTBRACKET:		return KeySym::RIGHTBRACKET;
		case SDLK_CARET:			return KeySym::CARET;
		case SDLK_UNDERSCORE:		return KeySym::UNDERSCORE;
		case SDLK_BACKQUOTE:		return KeySym::BACKQUOTE;
		case SDLK_a:				return KeySym::A;
		case SDLK_b:				return KeySym::B;
		case SDLK_c:				return KeySym::C;
		case SDLK_d:				return KeySym::D;
		case SDLK_e:				return KeySym::E;
		case SDLK_f:				return KeySym::F;
		case SDLK_g:				return KeySym::G;
		case SDLK_h:				return KeySym::H;
		case SDLK_i:				return KeySym::I;
		case SDLK_j:				return KeySym::J;
		case SDLK_k:				return KeySym::K;
		case SDLK_l:				return KeySym::L;
		case SDLK_m:				return KeySym::M;
		case SDLK_n:				return KeySym::N;
		case SDLK_o:				return KeySym::O;
		case SDLK_p:				return KeySym::P;
		case SDLK_q:				return KeySym::Q;
		case SDLK_r:				return KeySym::R;
		case SDLK_s:				return KeySym::S;
		case SDLK_t:				return KeySym::T;
		case SDLK_u:				return KeySym::U;
		case SDLK_v:				return KeySym::V;
		case SDLK_w:				return KeySym::W;
		case SDLK_x:				return KeySym::X;
		case SDLK_y:				return KeySym::Y;
		case SDLK_z:				return KeySym::Z;
		case SDLK_DELETE:			return KeySym::DELETE;

		case SDLK_KP_0:				return KeySym::KP0;
		case SDLK_KP_1:				return KeySym::KP1;
		case SDLK_KP_2:				return KeySym::KP2;
		case SDLK_KP_3:				return KeySym::KP3;
		case SDLK_KP_4:				return KeySym::KP4;
		case SDLK_KP_5:				return KeySym::KP5;
		case SDLK_KP_6:				return KeySym::KP6;
		case SDLK_KP_7:				return KeySym::KP7;
		case SDLK_KP_8:				return KeySym::KP8;
		case SDLK_KP_9:				return KeySym::KP9;
		case SDLK_KP_PERIOD:		return KeySym::KP_PERIOD;
		case SDLK_KP_DIVIDE:		return KeySym::KP_DIVIDE;
		case SDLK_KP_MULTIPLY:		return KeySym::KP_MULTIPLY;
		case SDLK_KP_MINUS:			return KeySym::KP_MINUS;
		case SDLK_KP_PLUS:			return KeySym::KP_PLUS;
		case SDLK_KP_ENTER:			return KeySym::KP_ENTER;
		case SDLK_KP_EQUALS:		return KeySym::KP_EQUALS;

		case SDLK_UP:				return KeySym::UP;
		case SDLK_DOWN:				return KeySym::DOWN;
		case SDLK_RIGHT:			return KeySym::RIGHT;
		case SDLK_LEFT:				return KeySym::LEFT;
		case SDLK_INSERT:			return KeySym::INSERT;
		case SDLK_HOME:				return KeySym::HOME;
		case SDLK_END:				return KeySym::END;
		case SDLK_PAGEUP:			return KeySym::PAGEUP;
		case SDLK_PAGEDOWN:			return KeySym::PAGEDOWN;

		case SDLK_F1:				return KeySym::F1;
		case SDLK_F2:				return KeySym::F2;
		case SDLK_F3:				return KeySym::F3;
		case SDLK_F4:				return KeySym::F4;
		case SDLK_F5:				return KeySym::F5;
		case SDLK_F6:				return KeySym::F6;
		case SDLK_F7:				return KeySym::F7;
		case SDLK_F8:				return KeySym::F8;
		case SDLK_F9:				return KeySym::F9;
		case SDLK_F10:				return KeySym::F10;
		case SDLK_F11:				return KeySym::F11;
		case SDLK_F12:				return KeySym::F12;
		case SDLK_F13:				return KeySym::F13;
		case SDLK_F14:				return KeySym::F14;
		case SDLK_F15:				return KeySym::F15;

		case SDLK_NUMLOCKCLEAR:		return KeySym::NUM_LOCK;
		case SDLK_CAPSLOCK:			return KeySym::CAPS_LOCK;
		case SDLK_SCROLLLOCK:		return KeySym::SCROLL_LOCK;
		case SDLK_RSHIFT:			return KeySym::RSHIFT;
		case SDLK_LSHIFT:			return KeySym::LSHIFT;
		case SDLK_RCTRL:			return KeySym::RCTRL;
		case SDLK_LCTRL:			return KeySym::LCTRL;
		case SDLK_RALT:				return KeySym::RALT;
		case SDLK_LALT:				return KeySym::LALT;
		case SDLK_RGUI:				return KeySym::RSUPER;
		case SDLK_LGUI:				return KeySym::LSUPER;
		case SDLK_MODE:				return KeySym::MODE;
		case SDLK_APPLICATION:		return KeySym::APPLICATION;

		case SDLK_HELP:				return KeySym::HELP;
		case SDLK_PRINTSCREEN:		return KeySym::PRINTSCREEN;
		case SDLK_SYSREQ:			return KeySym::SYSREQ;
		case SDLK_MENU:				return KeySym::MENU;
		case SDLK_POWER:			return KeySym::POWER;
		case SDLK_UNDO:				return KeySym::UNDO;

		default:					return KeySym::UNKNOWN;
	}
	// clang-format on
}

int SdlKeys::keyModMaskToEnumMask(int keymod)
{
	int result = 0;

	if (keymod != 0)
	{
		result |= (keymod & KMOD_LSHIFT) ? KeyMod::LSHIFT : 0;
		result |= (keymod & KMOD_RSHIFT) ? KeyMod::RSHIFT : 0;
		result |= (keymod & KMOD_LCTRL) ? KeyMod::LCTRL : 0;
		result |= (keymod & KMOD_RCTRL) ? KeyMod::RCTRL : 0;
		result |= (keymod & KMOD_LALT) ? KeyMod::LALT : 0;
		result |= (keymod & KMOD_RALT) ? KeyMod::RALT : 0;
		result |= (keymod & KMOD_LGUI) ? KeyMod::LSUPER : 0;
		result |= (keymod & KMOD_RGUI) ? KeyMod::RSUPER : 0;
		result |= (keymod & KMOD_NUM) ? KeyMod::NUM : 0;
		result |= (keymod & KMOD_CAPS) ? KeyMod::CAPS : 0;
		result |= (keymod & KMOD_MODE) ? KeyMod::MODE : 0;
	}

	return result;
}

int SdlKeys::enumToKeySymValue(KeySym keysym)
{
	// clang-format off
	switch (keysym)
	{
		case KeySym::UNKNOWN:			return SDLK_UNKNOWN;
		case KeySym::BACKSPACE:			return SDLK_BACKSPACE;
		case KeySym::TAB:				return SDLK_TAB;
		case KeySym::CLEAR:				return SDLK_CLEAR;
		case KeySym::RETURN:			return SDLK_RETURN;
		case KeySym::PAUSE:				return SDLK_PAUSE;
		case KeySym::ESCAPE:			return SDLK_ESCAPE;
		case KeySym::SPACE:				return SDLK_SPACE;
		case KeySym::EXCLAIM:			return SDLK_EXCLAIM;
		case KeySym::QUOTEDBL:			return SDLK_QUOTEDBL;
		case KeySym::HASH:				return SDLK_HASH;
		case KeySym::DOLLAR:			return SDLK_DOLLAR;
		case KeySym::AMPERSAND:			return SDLK_AMPERSAND;
		case KeySym::QUOTE:				return SDLK_QUOTE;
		case KeySym::LEFTPAREN:			return SDLK_LEFTPAREN;
		case KeySym::RIGHTPAREN:		return SDLK_RIGHTPAREN;
		case KeySym::ASTERISK:			return SDLK_ASTERISK;
		case KeySym::PLUS:				return SDLK_PLUS;
		case KeySym::COMMA:				return SDLK_COMMA;
		case KeySym::MINUS:				return SDLK_MINUS;
		case KeySym::PERIOD:			return SDLK_PERIOD;
		case KeySym::SLASH:				return SDLK_SLASH;
		case KeySym::N0:				return SDLK_0;
		case KeySym::N1:				return SDLK_1;
		case KeySym::N2:				return SDLK_2;
		case KeySym::N3:				return SDLK_3;
		case KeySym::N4:				return SDLK_4;
		case KeySym::N5:				return SDLK_5;
		case KeySym::N6:				return SDLK_6;
		case KeySym::N7:				return SDLK_7;
		case KeySym::N8:				return SDLK_8;
		case KeySym::N9:				return SDLK_9;
		case KeySym::COLON:				return SDLK_COLON;
		case KeySym::SEMICOLON:			return SDLK_SEMICOLON;
		case KeySym::LESS:				return SDLK_LESS;
		case KeySym::EQUALS:			return SDLK_EQUALS;
		case KeySym::GREATER:			return SDLK_GREATER;
		case KeySym::QUESTION:			return SDLK_QUESTION;
		case KeySym::AT:				return SDLK_AT;

		case KeySym::LEFTBRACKET:		return SDLK_LEFTBRACKET;
		case KeySym::BACKSLASH:			return SDLK_BACKSLASH;
		case KeySym::RIGHTBRACKET:		return SDLK_RIGHTBRACKET;
		case KeySym::CARET:				return SDLK_CARET;
		case KeySym::UNDERSCORE:		return SDLK_UNDERSCORE;
		case KeySym::BACKQUOTE:			return SDLK_BACKQUOTE;
		case KeySym::A:					return SDLK_a;
		case KeySym::B:					return SDLK_b;
		case KeySym::C:					return SDLK_c;
		case KeySym::D:					return SDLK_d;
		case KeySym::E:					return SDLK_e;
		case KeySym::F:					return SDLK_f;
		case KeySym::G:					return SDLK_g;
		case KeySym::H:					return SDLK_h;
		case KeySym::I:					return SDLK_i;
		case KeySym::J:					return SDLK_j;
		case KeySym::K:					return SDLK_k;
		case KeySym::L:					return SDLK_l;
		case KeySym::M:					return SDLK_m;
		case KeySym::N:					return SDLK_n;
		case KeySym::O:					return SDLK_o;
		case KeySym::P:					return SDLK_p;
		case KeySym::Q:					return SDLK_q;
		case KeySym::R:					return SDLK_r;
		case KeySym::S:					return SDLK_s;
		case KeySym::T:					return SDLK_t;
		case KeySym::U:					return SDLK_u;
		case KeySym::V:					return SDLK_v;
		case KeySym::W:					return SDLK_w;
		case KeySym::X:					return SDLK_x;
		case KeySym::Y:					return SDLK_y;
		case KeySym::Z:					return SDLK_z;
		case KeySym::DELETE:			return SDLK_DELETE;

		case KeySym::KP0:				return SDLK_KP_0;
		case KeySym::KP1:				return SDLK_KP_1;
		case KeySym::KP2:				return SDLK_KP_2;
		case KeySym::KP3:				return SDLK_KP_3;
		case KeySym::KP4:				return SDLK_KP_4;
		case KeySym::KP5:				return SDLK_KP_5;
		case KeySym::KP6:				return SDLK_KP_6;
		case KeySym::KP7:				return SDLK_KP_7;
		case KeySym::KP8:				return SDLK_KP_8;
		case KeySym::KP9:				return SDLK_KP_9;
		case KeySym::KP_PERIOD:			return SDLK_KP_PERIOD;
		case KeySym::KP_DIVIDE:			return SDLK_KP_DIVIDE;
		case KeySym::KP_MULTIPLY:		return SDLK_KP_MULTIPLY;
		case KeySym::KP_MINUS:			return SDLK_KP_MINUS;
		case KeySym::KP_PLUS:			return SDLK_KP_PLUS;
		case KeySym::KP_ENTER:			return SDLK_KP_ENTER;
		case KeySym::KP_EQUALS:			return SDLK_KP_EQUALS;

		case KeySym::UP:				return SDLK_UP;
		case KeySym::DOWN:				return SDLK_DOWN;
		case KeySym::RIGHT:				return SDLK_RIGHT;
		case KeySym::LEFT:				return SDLK_LEFT;
		case KeySym::INSERT:			return SDLK_INSERT;
		case KeySym::HOME:				return SDLK_HOME;
		case KeySym::END:				return SDLK_END;
		case KeySym::PAGEUP:			return SDLK_PAGEUP;
		case KeySym::PAGEDOWN:			return SDLK_PAGEDOWN;

		case KeySym::F1:				return SDLK_F1;
		case KeySym::F2:				return SDLK_F2;
		case KeySym::F3:				return SDLK_F3;
		case KeySym::F4:				return SDLK_F4;
		case KeySym::F5:				return SDLK_F5;
		case KeySym::F6:				return SDLK_F6;
		case KeySym::F7:				return SDLK_F7;
		case KeySym::F8:				return SDLK_F8;
		case KeySym::F9:				return SDLK_F9;
		case KeySym::F10:				return SDLK_F10;
		case KeySym::F11:				return SDLK_F11;
		case KeySym::F12:				return SDLK_F12;
		case KeySym::F13:				return SDLK_F13;
		case KeySym::F14:				return SDLK_F14;
		case KeySym::F15:				return SDLK_F15;

		case KeySym::NUM_LOCK:			return SDLK_NUMLOCKCLEAR;
		case KeySym::CAPS_LOCK:			return SDLK_CAPSLOCK;
		case KeySym::SCROLL_LOCK:		return SDLK_SCROLLLOCK;
		case KeySym::RSHIFT:			return SDLK_RSHIFT;
		case KeySym::LSHIFT:			return SDLK_LSHIFT;
		case KeySym::RCTRL:				return SDLK_RCTRL;
		case KeySym::LCTRL:				return SDLK_LCTRL;
		case KeySym::RALT:				return SDLK_RALT;
		case KeySym::LALT:				return SDLK_LALT;
		case KeySym::RSUPER:			return SDLK_RGUI;
		case KeySym::LSUPER:			return SDLK_LGUI;
		case KeySym::MODE:				return SDLK_MODE;
		case KeySym::APPLICATION:		return SDLK_APPLICATION;

		case KeySym::HELP:				return SDLK_HELP;
		case KeySym::PRINTSCREEN:		return SDLK_PRINTSCREEN;
		case KeySym::SYSREQ:			return SDLK_SYSREQ;
		case KeySym::MENU:				return SDLK_MENU;
		case KeySym::POWER:				return SDLK_POWER;
		case KeySym::UNDO:				return SDLK_UNDO;

		default:						return SDLK_UNKNOWN;
	}
	// clang-format on
}

int SdlKeys::enumToScancode(KeySym keysym)
{
	// clang-format off
	switch (keysym)
	{
		case KeySym::UNKNOWN:			return SDL_SCANCODE_UNKNOWN;
		case KeySym::BACKSPACE:			return SDL_SCANCODE_BACKSPACE;
		case KeySym::TAB:				return SDL_SCANCODE_TAB;
		case KeySym::CLEAR:				return SDL_SCANCODE_CLEAR;
		case KeySym::RETURN:			return SDL_SCANCODE_RETURN;
		case KeySym::PAUSE:				return SDL_SCANCODE_PAUSE;
		case KeySym::ESCAPE:			return SDL_SCANCODE_ESCAPE;
		case KeySym::SPACE:				return SDL_SCANCODE_SPACE;
		case KeySym::EXCLAIM:			return SDL_SCANCODE_1; // not a scancode
		case KeySym::QUOTEDBL:			return SDL_SCANCODE_APOSTROPHE; // not a scancode
		case KeySym::HASH:				return SDL_SCANCODE_3; // not a scancode
		case KeySym::DOLLAR:			return SDL_SCANCODE_4; // not a scancode
		case KeySym::AMPERSAND:			return SDL_SCANCODE_7; // not a scancode
		case KeySym::QUOTE:				return SDL_SCANCODE_APOSTROPHE; // not a scancode
		case KeySym::LEFTPAREN:			return SDL_SCANCODE_9; // not a scancode
		case KeySym::RIGHTPAREN:		return SDL_SCANCODE_0; // not a scancode
		case KeySym::ASTERISK:			return SDL_SCANCODE_8; // not a scancode
		case KeySym::PLUS:				return SDL_SCANCODE_EQUALS; // not a scancode
		case KeySym::COMMA:				return SDL_SCANCODE_COMMA;
		case KeySym::MINUS:				return SDL_SCANCODE_MINUS;
		case KeySym::PERIOD:			return SDL_SCANCODE_PERIOD;
		case KeySym::SLASH:				return SDL_SCANCODE_SLASH;
		case KeySym::N0:				return SDL_SCANCODE_0;
		case KeySym::N1:				return SDL_SCANCODE_1;
		case KeySym::N2:				return SDL_SCANCODE_2;
		case KeySym::N3:				return SDL_SCANCODE_3;
		case KeySym::N4:				return SDL_SCANCODE_4;
		case KeySym::N5:				return SDL_SCANCODE_5;
		case KeySym::N6:				return SDL_SCANCODE_6;
		case KeySym::N7:				return SDL_SCANCODE_7;
		case KeySym::N8:				return SDL_SCANCODE_8;
		case KeySym::N9:				return SDL_SCANCODE_9;
		case KeySym::COLON:				return SDL_SCANCODE_SEMICOLON; // not a scancode
		case KeySym::SEMICOLON:			return SDL_SCANCODE_SEMICOLON;
		case KeySym::LESS:				return SDL_SCANCODE_COMMA; // not a scancode
		case KeySym::EQUALS:			return SDL_SCANCODE_EQUALS;
		case KeySym::GREATER:			return SDL_SCANCODE_PERIOD; // not a scancode
		case KeySym::QUESTION:			return SDL_SCANCODE_SLASH; // not a scancode
		case KeySym::AT:				return SDL_SCANCODE_2; // not a scancode

		case KeySym::LEFTBRACKET:		return SDL_SCANCODE_LEFTBRACKET;
		case KeySym::BACKSLASH:			return SDL_SCANCODE_BACKSLASH;
		case KeySym::RIGHTBRACKET:		return SDL_SCANCODE_RIGHTBRACKET;
		case KeySym::CARET:				return SDL_SCANCODE_6; // not a scancode
		case KeySym::UNDERSCORE:		return SDL_SCANCODE_MINUS; // not a scancode
		case KeySym::BACKQUOTE:			return SDL_SCANCODE_GRAVE; // not a scancode
		case KeySym::A:					return SDL_SCANCODE_A;
		case KeySym::B:					return SDL_SCANCODE_B;
		case KeySym::C:					return SDL_SCANCODE_C;
		case KeySym::D:					return SDL_SCANCODE_D;
		case KeySym::E:					return SDL_SCANCODE_E;
		case KeySym::F:					return SDL_SCANCODE_F;
		case KeySym::G:					return SDL_SCANCODE_G;
		case KeySym::H:					return SDL_SCANCODE_H;
		case KeySym::I:					return SDL_SCANCODE_I;
		case KeySym::J:					return SDL_SCANCODE_J;
		case KeySym::K:					return SDL_SCANCODE_K;
		case KeySym::L:					return SDL_SCANCODE_L;
		case KeySym::M:					return SDL_SCANCODE_M;
		case KeySym::N:					return SDL_SCANCODE_N;
		case KeySym::O:					return SDL_SCANCODE_O;
		case KeySym::P:					return SDL_SCANCODE_P;
		case KeySym::Q:					return SDL_SCANCODE_Q;
		case KeySym::R:					return SDL_SCANCODE_R;
		case KeySym::S:					return SDL_SCANCODE_S;
		case KeySym::T:					return SDL_SCANCODE_T;
		case KeySym::U:					return SDL_SCANCODE_U;
		case KeySym::V:					return SDL_SCANCODE_V;
		case KeySym::W:					return SDL_SCANCODE_W;
		case KeySym::X:					return SDL_SCANCODE_X;
		case KeySym::Y:					return SDL_SCANCODE_Y;
		case KeySym::Z:					return SDL_SCANCODE_Z;
		case KeySym::DELETE:			return SDL_SCANCODE_DELETE;

		case KeySym::KP0:				return SDL_SCANCODE_KP_0;
		case KeySym::KP1:				return SDL_SCANCODE_KP_1;
		case KeySym::KP2:				return SDL_SCANCODE_KP_2;
		case KeySym::KP3:				return SDL_SCANCODE_KP_3;
		case KeySym::KP4:				return SDL_SCANCODE_KP_4;
		case KeySym::KP5:				return SDL_SCANCODE_KP_5;
		case KeySym::KP6:				return SDL_SCANCODE_KP_6;
		case KeySym::KP7:				return SDL_SCANCODE_KP_7;
		case KeySym::KP8:				return SDL_SCANCODE_KP_8;
		case KeySym::KP9:				return SDL_SCANCODE_KP_9;
		case KeySym::KP_PERIOD:			return SDL_SCANCODE_KP_PERIOD;
		case KeySym::KP_DIVIDE:			return SDL_SCANCODE_KP_DIVIDE;
		case KeySym::KP_MULTIPLY:		return SDL_SCANCODE_KP_MULTIPLY;
		case KeySym::KP_MINUS:			return SDL_SCANCODE_KP_MINUS;
		case KeySym::KP_PLUS:			return SDL_SCANCODE_KP_PLUS;
		case KeySym::KP_ENTER:			return SDL_SCANCODE_KP_ENTER;
		case KeySym::KP_EQUALS:			return SDL_SCANCODE_KP_EQUALS;

		case KeySym::UP:				return SDL_SCANCODE_UP;
		case KeySym::DOWN:				return SDL_SCANCODE_DOWN;
		case KeySym::RIGHT:				return SDL_SCANCODE_RIGHT;
		case KeySym::LEFT:				return SDL_SCANCODE_LEFT;
		case KeySym::INSERT:			return SDL_SCANCODE_INSERT;
		case KeySym::HOME:				return SDL_SCANCODE_HOME;
		case KeySym::END:				return SDL_SCANCODE_END;
		case KeySym::PAGEUP:			return SDL_SCANCODE_PAGEUP;
		case KeySym::PAGEDOWN:			return SDL_SCANCODE_PAGEDOWN;

		case KeySym::F1:				return SDL_SCANCODE_F1;
		case KeySym::F2:				return SDL_SCANCODE_F2;
		case KeySym::F3:				return SDL_SCANCODE_F3;
		case KeySym::F4:				return SDL_SCANCODE_F4;
		case KeySym::F5:				return SDL_SCANCODE_F5;
		case KeySym::F6:				return SDL_SCANCODE_F6;
		case KeySym::F7:				return SDL_SCANCODE_F7;
		case KeySym::F8:				return SDL_SCANCODE_F8;
		case KeySym::F9:				return SDL_SCANCODE_F9;
		case KeySym::F10:				return SDL_SCANCODE_F10;
		case KeySym::F11:				return SDL_SCANCODE_F11;
		case KeySym::F12:				return SDL_SCANCODE_F12;
		case KeySym::F13:				return SDL_SCANCODE_F13;
		case KeySym::F14:				return SDL_SCANCODE_F14;
		case KeySym::F15:				return SDL_SCANCODE_F15;

		case KeySym::NUM_LOCK:			return SDL_SCANCODE_NUMLOCKCLEAR;
		case KeySym::CAPS_LOCK:			return SDL_SCANCODE_CAPSLOCK;
		case KeySym::SCROLL_LOCK:		return SDL_SCANCODE_SCROLLLOCK;
		case KeySym::RSHIFT:			return SDL_SCANCODE_RSHIFT;
		case KeySym::LSHIFT:			return SDL_SCANCODE_LSHIFT;
		case KeySym::RCTRL:				return SDL_SCANCODE_RCTRL;
		case KeySym::LCTRL:				return SDL_SCANCODE_LCTRL;
		case KeySym::RALT:				return SDL_SCANCODE_RALT;
		case KeySym::LALT:				return SDL_SCANCODE_LALT;
		case KeySym::RSUPER:			return SDL_SCANCODE_RGUI;
		case KeySym::LSUPER:			return SDL_SCANCODE_LGUI;
		case KeySym::MODE:				return SDL_SCANCODE_MODE;
		case KeySym::APPLICATION:		return SDL_SCANCODE_APPLICATION;

		case KeySym::HELP:				return SDL_SCANCODE_HELP;
		case KeySym::PRINTSCREEN:		return SDL_SCANCODE_PRINTSCREEN;
		case KeySym::SYSREQ:			return SDL_SCANCODE_SYSREQ;
		case KeySym::MENU:				return SDL_SCANCODE_MENU;
		case KeySym::POWER:				return SDL_SCANCODE_POWER;
		case KeySym::UNDO:				return SDL_SCANCODE_UNDO;

		default:						return SDL_SCANCODE_UNKNOWN;
	}
	// clang-format on
}

}
