#include "GlfwInputManager.h"

namespace ncine {

KeySym GlfwKeys::keySymValueToEnum(int keysym)
{
	switch(keysym)
	{
		case GLFW_KEY_UNKNOWN:		return KEY_UNKNOWN;
		case GLFW_KEY_BACKSPACE:	return KEY_BACKSPACE;
		case GLFW_KEY_TAB:			return KEY_TAB;
		case GLFW_KEY_ENTER:		return KEY_RETURN;
		case GLFW_KEY_ESCAPE:		return KEY_ESCAPE;
		case GLFW_KEY_SPACE:		return KEY_SPACE;
		case GLFW_KEY_APOSTROPHE:	return KEY_QUOTE;
		case GLFW_KEY_EQUAL:		return KEY_PLUS;
		case GLFW_KEY_COMMA:		return KEY_COMMA;
		case GLFW_KEY_MINUS:		return KEY_MINUS;
		case GLFW_KEY_PERIOD:		return KEY_PERIOD;
		case GLFW_KEY_SLASH:		return KEY_SLASH;
		case GLFW_KEY_0:			return KEY_0;
		case GLFW_KEY_1:			return KEY_1;
		case GLFW_KEY_2:			return KEY_2;
		case GLFW_KEY_3:			return KEY_3;
		case GLFW_KEY_4:			return KEY_4;
		case GLFW_KEY_5:			return KEY_5;
		case GLFW_KEY_6:			return KEY_6;
		case GLFW_KEY_7:			return KEY_7;
		case GLFW_KEY_8:			return KEY_8;
		case GLFW_KEY_9:			return KEY_9;
		case GLFW_KEY_SEMICOLON:	return KEY_SEMICOLON;

		case GLFW_KEY_LEFT_BRACKET:	return KEY_LEFTBRACKET;
		case GLFW_KEY_BACKSLASH:	return KEY_BACKSLASH;
		case GLFW_KEY_RIGHT_BRACKET:return KEY_RIGHTBRACKET;
		case GLFW_KEY_GRAVE_ACCENT:	return KEY_BACKQUOTE;
		case GLFW_KEY_WORLD_1:		return KEY_WORLD1;
		case GLFW_KEY_WORLD_2:		return KEY_WORLD2;
		case GLFW_KEY_A:			return KEY_A;
		case GLFW_KEY_B:			return KEY_B;
		case GLFW_KEY_C:			return KEY_C;
		case GLFW_KEY_D:			return KEY_D;
		case GLFW_KEY_E:			return KEY_E;
		case GLFW_KEY_F:			return KEY_F;
		case GLFW_KEY_G:			return KEY_G;
		case GLFW_KEY_H:			return KEY_H;
		case GLFW_KEY_I:			return KEY_I;
		case GLFW_KEY_J:			return KEY_J;
		case GLFW_KEY_K:			return KEY_K;
		case GLFW_KEY_L:			return KEY_L;
		case GLFW_KEY_M:			return KEY_M;
		case GLFW_KEY_N:			return KEY_N;
		case GLFW_KEY_O:			return KEY_O;
		case GLFW_KEY_P:			return KEY_P;
		case GLFW_KEY_Q:			return KEY_Q;
		case GLFW_KEY_R:			return KEY_R;
		case GLFW_KEY_S:			return KEY_S;
		case GLFW_KEY_T:			return KEY_T;
		case GLFW_KEY_U:			return KEY_U;
		case GLFW_KEY_V:			return KEY_V;
		case GLFW_KEY_W:			return KEY_W;
		case GLFW_KEY_X:			return KEY_X;
		case GLFW_KEY_Y:			return KEY_Y;
		case GLFW_KEY_Z:			return KEY_Z;
		case GLFW_KEY_DELETE:		return KEY_DELETE;

		case GLFW_KEY_KP_0:			return KEY_KP0;
		case GLFW_KEY_KP_1:			return KEY_KP1;
		case GLFW_KEY_KP_2:			return KEY_KP2;
		case GLFW_KEY_KP_3:			return KEY_KP3;
		case GLFW_KEY_KP_4:			return KEY_KP4;
		case GLFW_KEY_KP_5:			return KEY_KP5;
		case GLFW_KEY_KP_6:			return KEY_KP6;
		case GLFW_KEY_KP_7:			return KEY_KP7;
		case GLFW_KEY_KP_8:			return KEY_KP8;
		case GLFW_KEY_KP_9:			return KEY_KP9;
		case GLFW_KEY_KP_DECIMAL:	return KEY_KP_PERIOD;
		case GLFW_KEY_KP_DIVIDE:	return KEY_KP_DIVIDE;
		case GLFW_KEY_KP_MULTIPLY:	return KEY_KP_MULTIPLY;
		case GLFW_KEY_KP_SUBTRACT:	return KEY_KP_MINUS;
		case GLFW_KEY_KP_ADD:		return KEY_KP_PLUS;
		case GLFW_KEY_KP_ENTER:		return KEY_KP_ENTER;
		case GLFW_KEY_KP_EQUAL:		return KEY_KP_EQUALS;

		case GLFW_KEY_UP:			return KEY_UP;
		case GLFW_KEY_DOWN:			return KEY_DOWN;
		case GLFW_KEY_RIGHT:		return KEY_RIGHT;
		case GLFW_KEY_LEFT:			return KEY_LEFT;
		case GLFW_KEY_INSERT:		return KEY_INSERT;
		case GLFW_KEY_HOME:			return KEY_HOME;
		case GLFW_KEY_END:			return KEY_END;
		case GLFW_KEY_PAGE_UP:		return KEY_PAGEUP;
		case GLFW_KEY_PAGE_DOWN:	return KEY_PAGEDOWN;

		case GLFW_KEY_F1:			return KEY_F1;
		case GLFW_KEY_F2:			return KEY_F2;
		case GLFW_KEY_F3:			return KEY_F3;
		case GLFW_KEY_F4:			return KEY_F4;
		case GLFW_KEY_F5:			return KEY_F5;
		case GLFW_KEY_F6:			return KEY_F6;
		case GLFW_KEY_F7:			return KEY_F7;
		case GLFW_KEY_F8:			return KEY_F8;
		case GLFW_KEY_F9:			return KEY_F9;
		case GLFW_KEY_F10:			return KEY_F10;
		case GLFW_KEY_F11:			return KEY_F11;
		case GLFW_KEY_F12:			return KEY_F12;
		case GLFW_KEY_F13:			return KEY_F13;
		case GLFW_KEY_F14:			return KEY_F14;
		case GLFW_KEY_F15:			return KEY_F15;

		case GLFW_KEY_NUM_LOCK:		return KEY_NUMLOCK;
		case GLFW_KEY_CAPS_LOCK:	return KEY_CAPSLOCK;
		case GLFW_KEY_SCROLL_LOCK:	return KEY_SCROLLOCK;
		case GLFW_KEY_RIGHT_SHIFT:	return KEY_RSHIFT;
		case GLFW_KEY_LEFT_SHIFT:	return KEY_LSHIFT;
		case GLFW_KEY_RIGHT_CONTROL:return KEY_RCTRL;
		case GLFW_KEY_LEFT_CONTROL:	return KEY_LCTRL;
		case GLFW_KEY_RIGHT_ALT:	return KEY_RALT;
		case GLFW_KEY_LEFT_ALT:		return KEY_LALT;
		case GLFW_KEY_RIGHT_SUPER:	return KEY_RSUPER;
		case GLFW_KEY_LEFT_SUPER:	return KEY_LSUPER;

		case GLFW_KEY_PRINT_SCREEN:	return KEY_PRINT;
		case GLFW_KEY_PAUSE:		return KEY_BREAK;
		case GLFW_KEY_MENU:			return KEY_MENU;

		default:					return KEY_UNKNOWN;
	}
}

KeyMod GlfwKeys::keyModValueToEnum(int keymod)
{
	switch(keymod)
	{
		case 0x0000:				return KEYMOD_NONE;
		case GLFW_MOD_SHIFT:		return KEYMOD_SHIFT;
		case GLFW_MOD_CONTROL:		return KEYMOD_CTRL;
		case GLFW_MOD_ALT:			return KEYMOD_ALT;
		case GLFW_MOD_SUPER:		return KEYMOD_META;

		default:					return KEYMOD_NONE;
	}
}

int GlfwKeys::enumToKeySymValue(KeySym keysym)
{
	switch(keysym)
	{
		case KEY_UNKNOWN:			return GLFW_KEY_UNKNOWN;
		case KEY_BACKSPACE:			return GLFW_KEY_BACKSPACE;
		case KEY_TAB:				return GLFW_KEY_TAB;
		case KEY_RETURN:			return GLFW_KEY_ENTER;
		case KEY_ESCAPE:			return GLFW_KEY_ESCAPE;
		case KEY_SPACE:				return GLFW_KEY_SPACE;
		case KEY_QUOTE:				return GLFW_KEY_APOSTROPHE;
		case KEY_PLUS:				return GLFW_KEY_EQUAL;
		case KEY_COMMA:				return GLFW_KEY_COMMA;
		case KEY_MINUS:				return GLFW_KEY_MINUS;
		case KEY_PERIOD:			return GLFW_KEY_PERIOD;
		case KEY_SLASH:				return GLFW_KEY_SLASH;
		case KEY_0:					return GLFW_KEY_0;
		case KEY_1:					return GLFW_KEY_1;
		case KEY_2:					return GLFW_KEY_2;
		case KEY_3:					return GLFW_KEY_3;
		case KEY_4:					return GLFW_KEY_4;
		case KEY_5:					return GLFW_KEY_5;
		case KEY_6:					return GLFW_KEY_6;
		case KEY_7:					return GLFW_KEY_7;
		case KEY_8:					return GLFW_KEY_8;
		case KEY_9:					return GLFW_KEY_9;
		case KEY_SEMICOLON:			return GLFW_KEY_SEMICOLON;

		case KEY_LEFTBRACKET:		return GLFW_KEY_LEFT_BRACKET;
		case KEY_BACKSLASH:			return GLFW_KEY_BACKSLASH;
		case KEY_RIGHTBRACKET:		return GLFW_KEY_RIGHT_BRACKET;
		case KEY_BACKQUOTE:			return GLFW_KEY_GRAVE_ACCENT;
		case KEY_WORLD1:			return GLFW_KEY_WORLD_1;
		case KEY_WORLD2:			return GLFW_KEY_WORLD_2;
		case KEY_A:					return GLFW_KEY_A;
		case KEY_B:					return GLFW_KEY_B;
		case KEY_C:					return GLFW_KEY_C;
		case KEY_D:					return GLFW_KEY_D;
		case KEY_E:					return GLFW_KEY_E;
		case KEY_F:					return GLFW_KEY_F;
		case KEY_G:					return GLFW_KEY_G;
		case KEY_H:					return GLFW_KEY_H;
		case KEY_I:					return GLFW_KEY_I;
		case KEY_J:					return GLFW_KEY_J;
		case KEY_K:					return GLFW_KEY_K;
		case KEY_L:					return GLFW_KEY_L;
		case KEY_M:					return GLFW_KEY_M;
		case KEY_N:					return GLFW_KEY_N;
		case KEY_O:					return GLFW_KEY_O;
		case KEY_P:					return GLFW_KEY_P;
		case KEY_Q:					return GLFW_KEY_Q;
		case KEY_R:					return GLFW_KEY_R;
		case KEY_S:					return GLFW_KEY_S;
		case KEY_T:					return GLFW_KEY_T;
		case KEY_U:					return GLFW_KEY_U;
		case KEY_V:					return GLFW_KEY_V;
		case KEY_W:					return GLFW_KEY_W;
		case KEY_X:					return GLFW_KEY_X;
		case KEY_Y:					return GLFW_KEY_Y;
		case KEY_Z:					return GLFW_KEY_Z;
		case KEY_DELETE:			return GLFW_KEY_DELETE;

		case KEY_KP0:				return GLFW_KEY_KP_0;
		case KEY_KP1:				return GLFW_KEY_KP_1;
		case KEY_KP2:				return GLFW_KEY_KP_2;
		case KEY_KP3:				return GLFW_KEY_KP_3;
		case KEY_KP4:				return GLFW_KEY_KP_4;
		case KEY_KP5:				return GLFW_KEY_KP_5;
		case KEY_KP6:				return GLFW_KEY_KP_6;
		case KEY_KP7:				return GLFW_KEY_KP_7;
		case KEY_KP8:				return GLFW_KEY_KP_8;
		case KEY_KP9:				return GLFW_KEY_KP_9;
		case KEY_KP_PERIOD:			return GLFW_KEY_KP_DECIMAL;
		case KEY_KP_DIVIDE:			return GLFW_KEY_KP_DIVIDE;
		case KEY_KP_MULTIPLY:		return GLFW_KEY_KP_MULTIPLY;
		case KEY_KP_MINUS:			return GLFW_KEY_KP_SUBTRACT;
		case KEY_KP_PLUS:			return GLFW_KEY_KP_ADD;
		case KEY_KP_ENTER:			return GLFW_KEY_KP_ENTER;
		case KEY_KP_EQUALS:			return GLFW_KEY_KP_EQUAL;

		case KEY_UP:				return GLFW_KEY_UP;
		case KEY_DOWN:				return GLFW_KEY_DOWN;
		case KEY_RIGHT:				return GLFW_KEY_RIGHT;
		case KEY_LEFT:				return GLFW_KEY_LEFT;
		case KEY_INSERT:			return GLFW_KEY_INSERT;
		case KEY_HOME:				return GLFW_KEY_HOME;
		case KEY_END:				return GLFW_KEY_END;
		case KEY_PAGEUP:			return GLFW_KEY_PAGE_UP;
		case KEY_PAGEDOWN:			return GLFW_KEY_PAGE_DOWN;

		case KEY_F1:				return GLFW_KEY_F1;
		case KEY_F2:				return GLFW_KEY_F2;
		case KEY_F3:				return GLFW_KEY_F3;
		case KEY_F4:				return GLFW_KEY_F4;
		case KEY_F5:				return GLFW_KEY_F5;
		case KEY_F6:				return GLFW_KEY_F6;
		case KEY_F7:				return GLFW_KEY_F7;
		case KEY_F8:				return GLFW_KEY_F8;
		case KEY_F9:				return GLFW_KEY_F9;
		case KEY_F10:				return GLFW_KEY_F10;
		case KEY_F11:				return GLFW_KEY_F11;
		case KEY_F12:				return GLFW_KEY_F12;
		case KEY_F13:				return GLFW_KEY_F13;
		case KEY_F14:				return GLFW_KEY_F14;
		case KEY_F15:				return GLFW_KEY_F15;

		case KEY_NUMLOCK:			return GLFW_KEY_NUM_LOCK;
		case KEY_CAPSLOCK:			return GLFW_KEY_CAPS_LOCK;
		case KEY_SCROLLOCK:			return GLFW_KEY_SCROLL_LOCK;
		case KEY_RSHIFT:			return GLFW_KEY_RIGHT_SHIFT;
		case KEY_LSHIFT:			return GLFW_KEY_LEFT_SHIFT;
		case KEY_RCTRL:				return GLFW_KEY_RIGHT_CONTROL;
		case KEY_LCTRL:				return GLFW_KEY_LEFT_CONTROL;
		case KEY_RALT:				return GLFW_KEY_RIGHT_ALT;
		case KEY_LALT:				return GLFW_KEY_LEFT_ALT;
		case KEY_RSUPER:			return GLFW_KEY_RIGHT_SUPER;
		case KEY_LSUPER:			return GLFW_KEY_LEFT_SUPER;

		case KEY_PRINT:				return GLFW_KEY_PRINT_SCREEN;
		case KEY_BREAK:				return GLFW_KEY_PAUSE;
		case KEY_MENU:				return GLFW_KEY_MENU;

		default:					return GLFW_KEY_UNKNOWN;
	}
}

int GlfwKeys::enumToKeyModValue(KeyMod keymod)
{
	switch(keymod)
	{
		case KEYMOD_NONE:			return 0x0000;
		case KEYMOD_SHIFT:			return GLFW_MOD_SHIFT;
		case KEYMOD_CTRL:			return GLFW_MOD_CONTROL;
		case KEYMOD_ALT:			return GLFW_MOD_ALT;
		case KEYMOD_META:			return GLFW_MOD_SUPER;

		default:					return 0x0000;
	}
}

}
