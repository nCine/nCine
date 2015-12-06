#ifndef NCINE_GLFWKEYS
#define NCINE_GLFWKEYS

#include "GlfwInputManager.h"

namespace ncine {

KeySym GlfwInputManager::keySymValueToEnum(int keysym)
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
		case GLFW_KEY_LEFT_SUPER:	return KEY_LSUPER;
		case GLFW_KEY_RIGHT_SUPER:	return KEY_RSUPER;

		case GLFW_KEY_PRINT_SCREEN:	return KEY_PRINT;
		case GLFW_KEY_PAUSE:		return KEY_BREAK;
		case GLFW_KEY_MENU:			return KEY_MENU;

		default:					return KEY_UNKNOWN;
	}
}

KeyMod GlfwInputManager::keyModValueToEnum(int keymod)
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

}

#endif
