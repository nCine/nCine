#include <GLFW/glfw3.h>
#include "GlfwInputManager.h"

namespace ncine {

KeySym GlfwKeys::keySymValueToEnum(int keysym)
{
	// clang-format off
	switch (keysym)
	{
		case GLFW_KEY_UNKNOWN:			return KeySym::UNKNOWN;
		case GLFW_KEY_BACKSPACE:		return KeySym::BACKSPACE;
		case GLFW_KEY_TAB:				return KeySym::TAB;
		case GLFW_KEY_ENTER:			return KeySym::RETURN;
		case GLFW_KEY_ESCAPE:			return KeySym::ESCAPE;
		case GLFW_KEY_SPACE:			return KeySym::SPACE;
		case GLFW_KEY_APOSTROPHE:		return KeySym::QUOTE;
		case GLFW_KEY_EQUAL:			return KeySym::PLUS;
		case GLFW_KEY_COMMA:			return KeySym::COMMA;
		case GLFW_KEY_MINUS:			return KeySym::MINUS;
		case GLFW_KEY_PERIOD:			return KeySym::PERIOD;
		case GLFW_KEY_SLASH:			return KeySym::SLASH;
		case GLFW_KEY_0:				return KeySym::N0;
		case GLFW_KEY_1:				return KeySym::N1;
		case GLFW_KEY_2:				return KeySym::N2;
		case GLFW_KEY_3:				return KeySym::N3;
		case GLFW_KEY_4:				return KeySym::N4;
		case GLFW_KEY_5:				return KeySym::N5;
		case GLFW_KEY_6:				return KeySym::N6;
		case GLFW_KEY_7:				return KeySym::N7;
		case GLFW_KEY_8:				return KeySym::N8;
		case GLFW_KEY_9:				return KeySym::N9;
		case GLFW_KEY_SEMICOLON:		return KeySym::SEMICOLON;

		case GLFW_KEY_LEFT_BRACKET:		return KeySym::LEFTBRACKET;
		case GLFW_KEY_BACKSLASH:		return KeySym::BACKSLASH;
		case GLFW_KEY_RIGHT_BRACKET:	 return KeySym::RIGHTBRACKET;
		case GLFW_KEY_GRAVE_ACCENT:		return KeySym::BACKQUOTE;
		case GLFW_KEY_WORLD_1:			return KeySym::WORLD1;
		case GLFW_KEY_WORLD_2:			return KeySym::WORLD2;
		case GLFW_KEY_A:				return KeySym::A;
		case GLFW_KEY_B:				return KeySym::B;
		case GLFW_KEY_C:				return KeySym::C;
		case GLFW_KEY_D:				return KeySym::D;
		case GLFW_KEY_E:				return KeySym::E;
		case GLFW_KEY_F:				return KeySym::F;
		case GLFW_KEY_G:				return KeySym::G;
		case GLFW_KEY_H:				return KeySym::H;
		case GLFW_KEY_I:				return KeySym::I;
		case GLFW_KEY_J:				return KeySym::J;
		case GLFW_KEY_K:				return KeySym::K;
		case GLFW_KEY_L:				return KeySym::L;
		case GLFW_KEY_M:				return KeySym::M;
		case GLFW_KEY_N:				return KeySym::N;
		case GLFW_KEY_O:				return KeySym::O;
		case GLFW_KEY_P:				return KeySym::P;
		case GLFW_KEY_Q:				return KeySym::Q;
		case GLFW_KEY_R:				return KeySym::R;
		case GLFW_KEY_S:				return KeySym::S;
		case GLFW_KEY_T:				return KeySym::T;
		case GLFW_KEY_U:				return KeySym::U;
		case GLFW_KEY_V:				return KeySym::V;
		case GLFW_KEY_W:				return KeySym::W;
		case GLFW_KEY_X:				return KeySym::X;
		case GLFW_KEY_Y:				return KeySym::Y;
		case GLFW_KEY_Z:				return KeySym::Z;
		case GLFW_KEY_DELETE:			return KeySym::DELETE;

		case GLFW_KEY_KP_0:				return KeySym::KP0;
		case GLFW_KEY_KP_1:				return KeySym::KP1;
		case GLFW_KEY_KP_2:				return KeySym::KP2;
		case GLFW_KEY_KP_3:				return KeySym::KP3;
		case GLFW_KEY_KP_4:				return KeySym::KP4;
		case GLFW_KEY_KP_5:				return KeySym::KP5;
		case GLFW_KEY_KP_6:				return KeySym::KP6;
		case GLFW_KEY_KP_7:				return KeySym::KP7;
		case GLFW_KEY_KP_8:				return KeySym::KP8;
		case GLFW_KEY_KP_9:				return KeySym::KP9;
		case GLFW_KEY_KP_DECIMAL:		return KeySym::KP_PERIOD;
		case GLFW_KEY_KP_DIVIDE:		return KeySym::KP_DIVIDE;
		case GLFW_KEY_KP_MULTIPLY:		return KeySym::KP_MULTIPLY;
		case GLFW_KEY_KP_SUBTRACT:		return KeySym::KP_MINUS;
		case GLFW_KEY_KP_ADD:			return KeySym::KP_PLUS;
		case GLFW_KEY_KP_ENTER:			return KeySym::KP_ENTER;
		case GLFW_KEY_KP_EQUAL:			return KeySym::KP_EQUALS;

		case GLFW_KEY_UP:				return KeySym::UP;
		case GLFW_KEY_DOWN:				return KeySym::DOWN;
		case GLFW_KEY_RIGHT:			return KeySym::RIGHT;
		case GLFW_KEY_LEFT:				return KeySym::LEFT;
		case GLFW_KEY_INSERT:			return KeySym::INSERT;
		case GLFW_KEY_HOME:				return KeySym::HOME;
		case GLFW_KEY_END:				return KeySym::END;
		case GLFW_KEY_PAGE_UP:			return KeySym::PAGEUP;
		case GLFW_KEY_PAGE_DOWN:		return KeySym::PAGEDOWN;

		case GLFW_KEY_F1:				return KeySym::F1;
		case GLFW_KEY_F2:				return KeySym::F2;
		case GLFW_KEY_F3:				return KeySym::F3;
		case GLFW_KEY_F4:				return KeySym::F4;
		case GLFW_KEY_F5:				return KeySym::F5;
		case GLFW_KEY_F6:				return KeySym::F6;
		case GLFW_KEY_F7:				return KeySym::F7;
		case GLFW_KEY_F8:				return KeySym::F8;
		case GLFW_KEY_F9:				return KeySym::F9;
		case GLFW_KEY_F10:				return KeySym::F10;
		case GLFW_KEY_F11:				return KeySym::F11;
		case GLFW_KEY_F12:				return KeySym::F12;
		case GLFW_KEY_F13:				return KeySym::F13;
		case GLFW_KEY_F14:				return KeySym::F14;
		case GLFW_KEY_F15:				return KeySym::F15;

		case GLFW_KEY_NUM_LOCK:			return KeySym::NUM_LOCK;
		case GLFW_KEY_CAPS_LOCK:		return KeySym::CAPS_LOCK;
		case GLFW_KEY_SCROLL_LOCK:		return KeySym::SCROLL_LOCK;
		case GLFW_KEY_RIGHT_SHIFT:		return KeySym::RSHIFT;
		case GLFW_KEY_LEFT_SHIFT:		return KeySym::LSHIFT;
		case GLFW_KEY_RIGHT_CONTROL:	return KeySym::RCTRL;
		case GLFW_KEY_LEFT_CONTROL:		return KeySym::LCTRL;
		case GLFW_KEY_RIGHT_ALT:		return KeySym::RALT;
		case GLFW_KEY_LEFT_ALT:			return KeySym::LALT;
		case GLFW_KEY_RIGHT_SUPER:		return KeySym::RSUPER;
		case GLFW_KEY_LEFT_SUPER:		return KeySym::LSUPER;

		case GLFW_KEY_PRINT_SCREEN:		return KeySym::PRINTSCREEN;
		case GLFW_KEY_PAUSE:			return KeySym::PAUSE;
		case GLFW_KEY_MENU:				return KeySym::MENU;

		default:						return KeySym::UNKNOWN;
	}
	// clang-format on
}

int GlfwKeys::keyModMaskToEnumMask(int keymod)
{
	int result = 0;

	if (keymod != 0)
	{
		result |= (keymod & GLFW_MOD_SHIFT) ? KeyMod::SHIFT : 0;
		result |= (keymod & GLFW_MOD_CONTROL) ? KeyMod::CTRL : 0;
		result |= (keymod & GLFW_MOD_ALT) ? KeyMod::ALT : 0;
		result |= (keymod & GLFW_MOD_SUPER) ? KeyMod::SUPER : 0;
	}

	return result;
}

int GlfwKeys::enumToKeySymValue(KeySym keysym)
{
	// clang-format off
	switch (keysym)
	{
		case KeySym::UNKNOWN:				return GLFW_KEY_UNKNOWN;
		case KeySym::BACKSPACE:				return GLFW_KEY_BACKSPACE;
		case KeySym::TAB:					return GLFW_KEY_TAB;
		case KeySym::RETURN:				return GLFW_KEY_ENTER;
		case KeySym::ESCAPE:				return GLFW_KEY_ESCAPE;
		case KeySym::SPACE:					return GLFW_KEY_SPACE;
		case KeySym::QUOTE:					return GLFW_KEY_APOSTROPHE;
		case KeySym::PLUS:					return GLFW_KEY_EQUAL;
		case KeySym::COMMA:					return GLFW_KEY_COMMA;
		case KeySym::MINUS:					return GLFW_KEY_MINUS;
		case KeySym::PERIOD:				return GLFW_KEY_PERIOD;
		case KeySym::SLASH:					return GLFW_KEY_SLASH;
		case KeySym::N0:					return GLFW_KEY_0;
		case KeySym::N1:					return GLFW_KEY_1;
		case KeySym::N2:					return GLFW_KEY_2;
		case KeySym::N3:					return GLFW_KEY_3;
		case KeySym::N4:					return GLFW_KEY_4;
		case KeySym::N5:					return GLFW_KEY_5;
		case KeySym::N6:					return GLFW_KEY_6;
		case KeySym::N7:					return GLFW_KEY_7;
		case KeySym::N8:					return GLFW_KEY_8;
		case KeySym::N9:					return GLFW_KEY_9;
		case KeySym::SEMICOLON:				return GLFW_KEY_SEMICOLON;

		case KeySym::LEFTBRACKET:			return GLFW_KEY_LEFT_BRACKET;
		case KeySym::BACKSLASH:				return GLFW_KEY_BACKSLASH;
		case KeySym::RIGHTBRACKET:			return GLFW_KEY_RIGHT_BRACKET;
		case KeySym::BACKQUOTE:				return GLFW_KEY_GRAVE_ACCENT;
		case KeySym::WORLD1:				return GLFW_KEY_WORLD_1;
		case KeySym::WORLD2:				return GLFW_KEY_WORLD_2;
		case KeySym::A:						return GLFW_KEY_A;
		case KeySym::B:						return GLFW_KEY_B;
		case KeySym::C:						return GLFW_KEY_C;
		case KeySym::D:						return GLFW_KEY_D;
		case KeySym::E:						return GLFW_KEY_E;
		case KeySym::F:						return GLFW_KEY_F;
		case KeySym::G:						return GLFW_KEY_G;
		case KeySym::H:						return GLFW_KEY_H;
		case KeySym::I:						return GLFW_KEY_I;
		case KeySym::J:						return GLFW_KEY_J;
		case KeySym::K:						return GLFW_KEY_K;
		case KeySym::L:						return GLFW_KEY_L;
		case KeySym::M:						return GLFW_KEY_M;
		case KeySym::N:						return GLFW_KEY_N;
		case KeySym::O:						return GLFW_KEY_O;
		case KeySym::P:						return GLFW_KEY_P;
		case KeySym::Q:						return GLFW_KEY_Q;
		case KeySym::R:						return GLFW_KEY_R;
		case KeySym::S:						return GLFW_KEY_S;
		case KeySym::T:						return GLFW_KEY_T;
		case KeySym::U:						return GLFW_KEY_U;
		case KeySym::V:						return GLFW_KEY_V;
		case KeySym::W:						return GLFW_KEY_W;
		case KeySym::X:						return GLFW_KEY_X;
		case KeySym::Y:						return GLFW_KEY_Y;
		case KeySym::Z:						return GLFW_KEY_Z;
		case KeySym::DELETE:				return GLFW_KEY_DELETE;

		case KeySym::KP0:					return GLFW_KEY_KP_0;
		case KeySym::KP1:					return GLFW_KEY_KP_1;
		case KeySym::KP2:					return GLFW_KEY_KP_2;
		case KeySym::KP3:					return GLFW_KEY_KP_3;
		case KeySym::KP4:					return GLFW_KEY_KP_4;
		case KeySym::KP5:					return GLFW_KEY_KP_5;
		case KeySym::KP6:					return GLFW_KEY_KP_6;
		case KeySym::KP7:					return GLFW_KEY_KP_7;
		case KeySym::KP8:					return GLFW_KEY_KP_8;
		case KeySym::KP9:					return GLFW_KEY_KP_9;
		case KeySym::KP_PERIOD:				return GLFW_KEY_KP_DECIMAL;
		case KeySym::KP_DIVIDE:				return GLFW_KEY_KP_DIVIDE;
		case KeySym::KP_MULTIPLY:			return GLFW_KEY_KP_MULTIPLY;
		case KeySym::KP_MINUS:				return GLFW_KEY_KP_SUBTRACT;
		case KeySym::KP_PLUS:				return GLFW_KEY_KP_ADD;
		case KeySym::KP_ENTER:				return GLFW_KEY_KP_ENTER;
		case KeySym::KP_EQUALS:				return GLFW_KEY_KP_EQUAL;

		case KeySym::UP:					return GLFW_KEY_UP;
		case KeySym::DOWN:					return GLFW_KEY_DOWN;
		case KeySym::RIGHT:					return GLFW_KEY_RIGHT;
		case KeySym::LEFT:					return GLFW_KEY_LEFT;
		case KeySym::INSERT:				return GLFW_KEY_INSERT;
		case KeySym::HOME:					return GLFW_KEY_HOME;
		case KeySym::END:					return GLFW_KEY_END;
		case KeySym::PAGEUP:				return GLFW_KEY_PAGE_UP;
		case KeySym::PAGEDOWN:				return GLFW_KEY_PAGE_DOWN;

		case KeySym::F1:					return GLFW_KEY_F1;
		case KeySym::F2:					return GLFW_KEY_F2;
		case KeySym::F3:					return GLFW_KEY_F3;
		case KeySym::F4:					return GLFW_KEY_F4;
		case KeySym::F5:					return GLFW_KEY_F5;
		case KeySym::F6:					return GLFW_KEY_F6;
		case KeySym::F7:					return GLFW_KEY_F7;
		case KeySym::F8:					return GLFW_KEY_F8;
		case KeySym::F9:					return GLFW_KEY_F9;
		case KeySym::F10:					return GLFW_KEY_F10;
		case KeySym::F11:					return GLFW_KEY_F11;
		case KeySym::F12:					return GLFW_KEY_F12;
		case KeySym::F13:					return GLFW_KEY_F13;
		case KeySym::F14:					return GLFW_KEY_F14;
		case KeySym::F15:					return GLFW_KEY_F15;

		case KeySym::NUM_LOCK:				return GLFW_KEY_NUM_LOCK;
		case KeySym::CAPS_LOCK:				return GLFW_KEY_CAPS_LOCK;
		case KeySym::SCROLL_LOCK:			return GLFW_KEY_SCROLL_LOCK;
		case KeySym::RSHIFT:				return GLFW_KEY_RIGHT_SHIFT;
		case KeySym::LSHIFT:				return GLFW_KEY_LEFT_SHIFT;
		case KeySym::RCTRL:					return GLFW_KEY_RIGHT_CONTROL;
		case KeySym::LCTRL:					return GLFW_KEY_LEFT_CONTROL;
		case KeySym::RALT:					return GLFW_KEY_RIGHT_ALT;
		case KeySym::LALT:					return GLFW_KEY_LEFT_ALT;
		case KeySym::RSUPER:				return GLFW_KEY_RIGHT_SUPER;
		case KeySym::LSUPER:				return GLFW_KEY_LEFT_SUPER;

		case KeySym::PRINTSCREEN:			return GLFW_KEY_PRINT_SCREEN;
		case KeySym::PAUSE:					return GLFW_KEY_PAUSE;
		case KeySym::MENU:					return GLFW_KEY_MENU;

		default:							return GLFW_KEY_UNKNOWN;
	}
	// clang-format on
}

}
