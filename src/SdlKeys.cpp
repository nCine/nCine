#include "SdlInputManager.h"

namespace ncine {

KeySym SdlKeys::keySymValueToEnum(int keysym)
{
	switch(keysym)
	{
		case SDLK_UNKNOWN:			return KEY_UNKNOWN;
		case SDLK_BACKSPACE:		return KEY_BACKSPACE;
		case SDLK_TAB:				return KEY_TAB;
		case SDLK_CLEAR:			return KEY_CLEAR;
		case SDLK_RETURN:			return KEY_RETURN;
		case SDLK_PAUSE:			return KEY_PAUSE;
		case SDLK_ESCAPE:			return KEY_ESCAPE;
		case SDLK_SPACE:			return KEY_SPACE;
		case SDLK_EXCLAIM:			return KEY_EXCLAIM;
		case SDLK_QUOTEDBL:			return KEY_QUOTEDBL;
		case SDLK_HASH:				return KEY_HASH;
		case SDLK_DOLLAR:			return KEY_DOLLAR;
		case SDLK_AMPERSAND:		return KEY_AMPERSAND;
		case SDLK_QUOTE:			return KEY_QUOTE;
		case SDLK_LEFTPAREN:		return KEY_LEFTPAREN;
		case SDLK_RIGHTPAREN:		return KEY_RIGHTPAREN;
		case SDLK_ASTERISK:			return KEY_ASTERISK;
		case SDLK_PLUS:				return KEY_PLUS;
		case SDLK_COMMA:			return KEY_COMMA;
		case SDLK_MINUS:			return KEY_MINUS;
		case SDLK_PERIOD:			return KEY_PERIOD;
		case SDLK_SLASH:			return KEY_SLASH;
		case SDLK_0:				return KEY_0;
		case SDLK_1:				return KEY_1;
		case SDLK_2:				return KEY_2;
		case SDLK_3:				return KEY_3;
		case SDLK_4:				return KEY_4;
		case SDLK_5:				return KEY_5;
		case SDLK_6:				return KEY_6;
		case SDLK_7:				return KEY_7;
		case SDLK_8:				return KEY_8;
		case SDLK_9:				return KEY_9;
		case SDLK_COLON:			return KEY_COLON;
		case SDLK_SEMICOLON:		return KEY_SEMICOLON;
		case SDLK_LESS:				return KEY_LESS;
		case SDLK_EQUALS:			return KEY_EQUALS;
		case SDLK_GREATER:			return KEY_GREATER;
		case SDLK_QUESTION:			return KEY_QUESTION;
		case SDLK_AT:				return KEY_AT;

		case SDLK_LEFTBRACKET:		return KEY_LEFTBRACKET;
		case SDLK_BACKSLASH:		return KEY_BACKSLASH;
		case SDLK_RIGHTBRACKET:		return KEY_RIGHTBRACKET;
		case SDLK_CARET:			return KEY_CARET;
		case SDLK_UNDERSCORE:		return KEY_UNDERSCORE;
		case SDLK_BACKQUOTE:		return KEY_BACKQUOTE;
		case SDLK_a:				return KEY_A;
		case SDLK_b:				return KEY_B;
		case SDLK_c:				return KEY_C;
		case SDLK_d:				return KEY_D;
		case SDLK_e:				return KEY_E;
		case SDLK_f:				return KEY_F;
		case SDLK_g:				return KEY_G;
		case SDLK_h:				return KEY_H;
		case SDLK_i:				return KEY_I;
		case SDLK_j:				return KEY_J;
		case SDLK_k:				return KEY_K;
		case SDLK_l:				return KEY_L;
		case SDLK_m:				return KEY_M;
		case SDLK_n:				return KEY_N;
		case SDLK_o:				return KEY_O;
		case SDLK_p:				return KEY_P;
		case SDLK_q:				return KEY_Q;
		case SDLK_r:				return KEY_R;
		case SDLK_s:				return KEY_S;
		case SDLK_t:				return KEY_T;
		case SDLK_u:				return KEY_U;
		case SDLK_v:				return KEY_V;
		case SDLK_w:				return KEY_W;
		case SDLK_x:				return KEY_X;
		case SDLK_y:				return KEY_Y;
		case SDLK_z:				return KEY_Z;
		case SDLK_DELETE:			return KEY_DELETE;

		case SDLK_KP0:				return KEY_KP0;
		case SDLK_KP1:				return KEY_KP1;
		case SDLK_KP2:				return KEY_KP2;
		case SDLK_KP3:				return KEY_KP3;
		case SDLK_KP4:				return KEY_KP4;
		case SDLK_KP5:				return KEY_KP5;
		case SDLK_KP6:				return KEY_KP6;
		case SDLK_KP7:				return KEY_KP7;
		case SDLK_KP8:				return KEY_KP8;
		case SDLK_KP9:				return KEY_KP9;
		case SDLK_KP_PERIOD:		return KEY_KP_PERIOD;
		case SDLK_KP_DIVIDE:		return KEY_KP_DIVIDE;
		case SDLK_KP_MULTIPLY:		return KEY_KP_MULTIPLY;
		case SDLK_KP_MINUS:			return KEY_KP_MINUS;
		case SDLK_KP_PLUS:			return KEY_KP_PLUS;
		case SDLK_KP_ENTER:			return KEY_KP_ENTER;
		case SDLK_KP_EQUALS:		return KEY_KP_EQUALS;

		case SDLK_UP:				return KEY_UP;
		case SDLK_DOWN:				return KEY_DOWN;
		case SDLK_RIGHT:			return KEY_RIGHT;
		case SDLK_LEFT:				return KEY_LEFT;
		case SDLK_INSERT:			return KEY_INSERT;
		case SDLK_HOME:				return KEY_HOME;
		case SDLK_END:				return KEY_END;
		case SDLK_PAGEUP:			return KEY_PAGEUP;
		case SDLK_PAGEDOWN:			return KEY_PAGEDOWN;

		case SDLK_F1:				return KEY_F1;
		case SDLK_F2:				return KEY_F2;
		case SDLK_F3:				return KEY_F3;
		case SDLK_F4:				return KEY_F4;
		case SDLK_F5:				return KEY_F5;
		case SDLK_F6:				return KEY_F6;
		case SDLK_F7:				return KEY_F7;
		case SDLK_F8:				return KEY_F8;
		case SDLK_F9:				return KEY_F9;
		case SDLK_F10:				return KEY_F10;
		case SDLK_F11:				return KEY_F11;
		case SDLK_F12:				return KEY_F12;
		case SDLK_F13:				return KEY_F13;
		case SDLK_F14:				return KEY_F14;
		case SDLK_F15:				return KEY_F15;

		case SDLK_NUMLOCK:			return KEY_NUMLOCK;
		case SDLK_CAPSLOCK:			return KEY_CAPSLOCK;
		case SDLK_SCROLLOCK:		return KEY_SCROLLOCK;
		case SDLK_RSHIFT:			return KEY_RSHIFT;
		case SDLK_LSHIFT:			return KEY_LSHIFT;
		case SDLK_RCTRL:			return KEY_RCTRL;
		case SDLK_LCTRL:			return KEY_LCTRL;
		case SDLK_RALT:				return KEY_RALT;
		case SDLK_LALT:				return KEY_LALT;
		case SDLK_RMETA:			return KEY_RMETA;
		case SDLK_LMETA:			return KEY_LMETA;
		case SDLK_LSUPER:			return KEY_LSUPER;
		case SDLK_RSUPER:			return KEY_RSUPER;
		case SDLK_MODE:				return KEY_MODE;
		case SDLK_COMPOSE:			return KEY_COMPOSE;

		case SDLK_HELP:				return KEY_HELP;
		case SDLK_PRINT:			return KEY_PRINT;
		case SDLK_SYSREQ:			return KEY_SYSREQ;
		case SDLK_BREAK:			return KEY_BREAK;
		case SDLK_MENU:				return KEY_MENU;
		case SDLK_POWER:			return KEY_POWER;
		case SDLK_EURO:				return KEY_EURO;
		case SDLK_UNDO:				return KEY_UNDO;

		default:					return KEY_UNKNOWN;
	}
}

KeyMod SdlKeys::keyModValueToEnum(int keymod)
{
	switch(keymod)
	{
		case KMOD_NONE:				return KEYMOD_NONE;
		case KMOD_LSHIFT:			return KEYMOD_LSHIFT;
		case KMOD_RSHIFT:			return KEYMOD_RSHIFT;
		case KMOD_LCTRL:			return KEYMOD_LCTRL;
		case KMOD_RCTRL:			return KEYMOD_RCTRL;
		case KMOD_LALT:				return KEYMOD_LALT;
		case KMOD_RALT:				return KEYMOD_RALT;
		case KMOD_LMETA:			return KEYMOD_LMETA;
		case KMOD_RMETA:			return KEYMOD_RMETA;
		case KMOD_NUM:				return KEYMOD_NUM;
		case KMOD_CAPS:				return KEYMOD_CAPS;
		case KMOD_MODE:				return KEYMOD_MODE;

		default:					return KEYMOD_NONE;
	}
}

int SdlKeys::enumToKeySymValue(KeySym keysym)
{
	switch(keysym)
	{
		case KEY_UNKNOWN:			return SDLK_UNKNOWN;
		case KEY_BACKSPACE:			return SDLK_BACKSPACE;
		case KEY_TAB:				return SDLK_TAB;
		case KEY_CLEAR:				return SDLK_CLEAR;
		case KEY_RETURN:			return SDLK_RETURN;
		case KEY_PAUSE:				return SDLK_PAUSE;
		case KEY_ESCAPE:			return SDLK_ESCAPE;
		case KEY_SPACE:				return SDLK_SPACE;
		case KEY_EXCLAIM:			return SDLK_EXCLAIM;
		case KEY_QUOTEDBL:			return SDLK_QUOTEDBL;
		case KEY_HASH:				return SDLK_HASH;
		case KEY_DOLLAR:			return SDLK_DOLLAR;
		case KEY_AMPERSAND:			return SDLK_AMPERSAND;
		case KEY_QUOTE:				return SDLK_QUOTE;
		case KEY_LEFTPAREN:			return SDLK_LEFTPAREN;
		case KEY_RIGHTPAREN:		return SDLK_RIGHTPAREN;
		case KEY_ASTERISK:			return SDLK_ASTERISK;
		case KEY_PLUS:				return SDLK_PLUS;
		case KEY_COMMA:				return SDLK_COMMA;
		case KEY_MINUS:				return SDLK_MINUS;
		case KEY_PERIOD:			return SDLK_PERIOD;
		case KEY_SLASH:				return SDLK_SLASH;
		case KEY_0:					return SDLK_0;
		case KEY_1:					return SDLK_1;
		case KEY_2:					return SDLK_2;
		case KEY_3:					return SDLK_3;
		case KEY_4:					return SDLK_4;
		case KEY_5:					return SDLK_5;
		case KEY_6:					return SDLK_6;
		case KEY_7:					return SDLK_7;
		case KEY_8:					return SDLK_8;
		case KEY_9:					return SDLK_9;
		case KEY_COLON:				return SDLK_COLON;
		case KEY_SEMICOLON:			return SDLK_SEMICOLON;
		case KEY_LESS:				return SDLK_LESS;
		case KEY_EQUALS:			return SDLK_EQUALS;
		case KEY_GREATER:			return SDLK_GREATER;
		case KEY_QUESTION:			return SDLK_QUESTION;
		case KEY_AT:				return SDLK_AT;

		case KEY_LEFTBRACKET:		return SDLK_LEFTBRACKET;
		case KEY_BACKSLASH:			return SDLK_BACKSLASH;
		case KEY_RIGHTBRACKET:		return SDLK_RIGHTBRACKET;
		case KEY_CARET:				return SDLK_CARET;
		case KEY_UNDERSCORE:		return SDLK_UNDERSCORE;
		case KEY_BACKQUOTE:			return SDLK_BACKQUOTE;
		case KEY_A:					return SDLK_a;
		case KEY_B:					return SDLK_b;
		case KEY_C:					return SDLK_c;
		case KEY_D:					return SDLK_d;
		case KEY_E:					return SDLK_e;
		case KEY_F:					return SDLK_f;
		case KEY_G:					return SDLK_g;
		case KEY_H:					return SDLK_h;
		case KEY_I:					return SDLK_i;
		case KEY_J:					return SDLK_j;
		case KEY_K:					return SDLK_k;
		case KEY_L:					return SDLK_l;
		case KEY_M:					return SDLK_m;
		case KEY_N:					return SDLK_n;
		case KEY_O:					return SDLK_o;
		case KEY_P:					return SDLK_p;
		case KEY_Q:					return SDLK_q;
		case KEY_R:					return SDLK_r;
		case KEY_S:					return SDLK_s;
		case KEY_T:					return SDLK_t;
		case KEY_U:					return SDLK_u;
		case KEY_V:					return SDLK_v;
		case KEY_W:					return SDLK_w;
		case KEY_X:					return SDLK_x;
		case KEY_Y:					return SDLK_y;
		case KEY_Z:					return SDLK_z;
		case KEY_DELETE:			return SDLK_DELETE;

		case KEY_KP0:				return SDLK_KP0;
		case KEY_KP1:				return SDLK_KP1;
		case KEY_KP2:				return SDLK_KP2;
		case KEY_KP3:				return SDLK_KP3;
		case KEY_KP4:				return SDLK_KP4;
		case KEY_KP5:				return SDLK_KP5;
		case KEY_KP6:				return SDLK_KP6;
		case KEY_KP7:				return SDLK_KP7;
		case KEY_KP8:				return SDLK_KP8;
		case KEY_KP9:				return SDLK_KP9;
		case KEY_KP_PERIOD:			return SDLK_KP_PERIOD;
		case KEY_KP_DIVIDE:			return SDLK_KP_DIVIDE;
		case KEY_KP_MULTIPLY:		return SDLK_KP_MULTIPLY;
		case KEY_KP_MINUS:			return SDLK_KP_MINUS;
		case KEY_KP_PLUS:			return SDLK_KP_PLUS;
		case KEY_KP_ENTER:			return SDLK_KP_ENTER;
		case KEY_KP_EQUALS:			return SDLK_KP_EQUALS;

		case KEY_UP:				return SDLK_UP;
		case KEY_DOWN:				return SDLK_DOWN;
		case KEY_RIGHT:				return SDLK_RIGHT;
		case KEY_LEFT:				return SDLK_LEFT;
		case KEY_INSERT:			return SDLK_INSERT;
		case KEY_HOME:				return SDLK_HOME;
		case KEY_END:				return SDLK_END;
		case KEY_PAGEUP:			return SDLK_PAGEUP;
		case KEY_PAGEDOWN:			return SDLK_PAGEDOWN;

		case KEY_F1:				return SDLK_F1;
		case KEY_F2:				return SDLK_F2;
		case KEY_F3:				return SDLK_F3;
		case KEY_F4:				return SDLK_F4;
		case KEY_F5:				return SDLK_F5;
		case KEY_F6:				return SDLK_F6;
		case KEY_F7:				return SDLK_F7;
		case KEY_F8:				return SDLK_F8;
		case KEY_F9:				return SDLK_F9;
		case KEY_F10:				return SDLK_F10;
		case KEY_F11:				return SDLK_F11;
		case KEY_F12:				return SDLK_F12;
		case KEY_F13:				return SDLK_F13;
		case KEY_F14:				return SDLK_F14;
		case KEY_F15:				return SDLK_F15;

		case KEY_NUMLOCK:			return SDLK_NUMLOCK;
		case KEY_CAPSLOCK:			return SDLK_CAPSLOCK;
		case KEY_SCROLLOCK:			return SDLK_SCROLLOCK;
		case KEY_RSHIFT:			return SDLK_RSHIFT;
		case KEY_LSHIFT:			return SDLK_LSHIFT;
		case KEY_RCTRL:				return SDLK_RCTRL;
		case KEY_LCTRL:				return SDLK_LCTRL;
		case KEY_RALT:				return SDLK_RALT;
		case KEY_LALT:				return SDLK_LALT;
		case KEY_RMETA:				return SDLK_RMETA;
		case KEY_LMETA:				return SDLK_LMETA;
		case KEY_LSUPER:			return SDLK_LSUPER;
		case KEY_RSUPER:			return SDLK_RSUPER;
		case KEY_MODE:				return SDLK_MODE;
		case KEY_COMPOSE:			return SDLK_COMPOSE;

		case KEY_HELP:				return SDLK_HELP;
		case KEY_PRINT:				return SDLK_PRINT;
		case KEY_SYSREQ:			return SDLK_SYSREQ;
		case KEY_BREAK:				return SDLK_BREAK;
		case KEY_MENU:				return SDLK_MENU;
		case KEY_POWER:				return SDLK_POWER;
		case KEY_EURO:				return SDLK_EURO;
		case KEY_UNDO:				return SDLK_UNDO;

		default:					return SDLK_UNKNOWN;
	}
}

int SdlKeys::enumToKeyModValue(KeyMod keymod)
{
	switch(keymod)
	{
		case KEYMOD_NONE:			return KMOD_NONE;
		case KEYMOD_LSHIFT:			return KMOD_LSHIFT;
		case KEYMOD_RSHIFT:			return KMOD_RSHIFT;
		case KEYMOD_LCTRL:			return KMOD_LCTRL;
		case KEYMOD_RCTRL:			return KMOD_RCTRL;
		case KEYMOD_LALT:			return KMOD_LALT;
		case KEYMOD_RALT:			return KMOD_RALT;
		case KEYMOD_LMETA:			return KMOD_LMETA;
		case KEYMOD_RMETA:			return KMOD_RMETA;
		case KEYMOD_NUM:			return KMOD_NUM;
		case KEYMOD_CAPS:			return KMOD_CAPS;
		case KEYMOD_MODE:			return KMOD_MODE;

		default:					return KMOD_NONE;
	}
}

}
