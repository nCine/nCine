#include "Qt5InputManager.h"

namespace ncine {

KeySym Qt5Keys::keySymValueToEnum(int keysym)
{
	// clang-format off
	switch (keysym)
	{
		case Qt::Key_unknown:			return KeySym::UNKNOWN;
		case Qt::Key_Backspace	:		return KeySym::BACKSPACE;
		case Qt::Key_Tab:				return KeySym::TAB;
		case Qt::Key_Clear:				return KeySym::CLEAR;
		case Qt::Key_Return:			return KeySym::RETURN;
		case Qt::Key_Pause:				return KeySym::PAUSE;
		case Qt::Key_Escape:			return KeySym::ESCAPE;
		case Qt::Key_Space:				return KeySym::SPACE;
		case Qt::Key_Exclam:			return KeySym::EXCLAIM;
		case Qt::Key_QuoteDbl:			return KeySym::QUOTEDBL;
		case Qt::Key_NumberSign:		return KeySym::HASH;
		case Qt::Key_Dollar:			return KeySym::DOLLAR;
		case Qt::Key_Ampersand:			return KeySym::AMPERSAND;
		case Qt::Key_Apostrophe:		return KeySym::QUOTE; // TEST
		case Qt::Key_ParenLeft:			return KeySym::LEFTPAREN;
		case Qt::Key_ParenRight:		return KeySym::RIGHTPAREN;
		case Qt::Key_Asterisk:			return KeySym::ASTERISK;
		case Qt::Key_Plus:				return KeySym::PLUS;
		case Qt::Key_Comma:				return KeySym::COMMA;
		case Qt::Key_Minus:				return KeySym::MINUS;
		case Qt::Key_Period:			return KeySym::PERIOD;
		case Qt::Key_Slash:				return KeySym::SLASH;
		case Qt::Key_0:					return KeySym::N0;
		case Qt::Key_1:					return KeySym::N1;
		case Qt::Key_2:					return KeySym::N2;
		case Qt::Key_3:					return KeySym::N3;
		case Qt::Key_4:					return KeySym::N4;
		case Qt::Key_5:					return KeySym::N5;
		case Qt::Key_6:					return KeySym::N6;
		case Qt::Key_7:					return KeySym::N7;
		case Qt::Key_8:					return KeySym::N8;
		case Qt::Key_9:					return KeySym::N9;
		case Qt::Key_Colon:				return KeySym::COLON;
		case Qt::Key_Semicolon:			return KeySym::SEMICOLON;
		case Qt::Key_Less:				return KeySym::LESS;
		case Qt::Key_Equal:				return KeySym::EQUALS;
		case Qt::Key_Greater:			return KeySym::GREATER;
		case Qt::Key_Question:			return KeySym::QUESTION;
		case Qt::Key_At:				return KeySym::AT;

		case Qt::Key_BracketLeft:		return KeySym::LEFTBRACKET;
		case Qt::Key_Backslash:			return KeySym::BACKSLASH;
		case Qt::Key_BracketRight:		return KeySym::RIGHTBRACKET;
		case Qt::Key_AsciiCircum:		return KeySym::CARET; // TEST
		case Qt::Key_Underscore:		return KeySym::UNDERSCORE;
		case Qt::Key_QuoteLeft:			return KeySym::BACKQUOTE;
		case Qt::Key_A:					return KeySym::A;
		case Qt::Key_B:					return KeySym::B;
		case Qt::Key_C:					return KeySym::C;
		case Qt::Key_D:					return KeySym::D;
		case Qt::Key_E:					return KeySym::E;
		case Qt::Key_F:					return KeySym::F;
		case Qt::Key_G:					return KeySym::G;
		case Qt::Key_H:					return KeySym::H;
		case Qt::Key_I:					return KeySym::I;
		case Qt::Key_J:					return KeySym::J;
		case Qt::Key_K:					return KeySym::K;
		case Qt::Key_L:					return KeySym::L;
		case Qt::Key_M:					return KeySym::M;
		case Qt::Key_N:					return KeySym::N;
		case Qt::Key_O:					return KeySym::O;
		case Qt::Key_P:					return KeySym::P;
		case Qt::Key_Q:					return KeySym::Q;
		case Qt::Key_R:					return KeySym::R;
		case Qt::Key_S:					return KeySym::S;
		case Qt::Key_T:					return KeySym::T;
		case Qt::Key_U:					return KeySym::U;
		case Qt::Key_V:					return KeySym::V;
		case Qt::Key_W:					return KeySym::W;
		case Qt::Key_X:					return KeySym::X;
		case Qt::Key_Y:					return KeySym::Y;
		case Qt::Key_Z:					return KeySym::Z;
		case Qt::Key_Delete:			return KeySym::DELETE;

		case Qt::Key_Enter:				return KeySym::KP_ENTER;

		case Qt::Key_Up:				return KeySym::UP;
		case Qt::Key_Down:				return KeySym::DOWN;
		case Qt::Key_Right:				return KeySym::RIGHT;
		case Qt::Key_Left:				return KeySym::LEFT;
		case Qt::Key_Insert:			return KeySym::INSERT;
		case Qt::Key_Home:				return KeySym::HOME;
		case Qt::Key_End:				return KeySym::END;
		case Qt::Key_PageUp:			return KeySym::PAGEUP;
		case Qt::Key_PageDown:			return KeySym::PAGEDOWN;

		case Qt::Key_F1:				return KeySym::F1;
		case Qt::Key_F2:				return KeySym::F2;
		case Qt::Key_F3:				return KeySym::F3;
		case Qt::Key_F4:				return KeySym::F4;
		case Qt::Key_F5:				return KeySym::F5;
		case Qt::Key_F6:				return KeySym::F6;
		case Qt::Key_F7:				return KeySym::F7;
		case Qt::Key_F8:				return KeySym::F8;
		case Qt::Key_F9:				return KeySym::F9;
		case Qt::Key_F10:				return KeySym::F10;
		case Qt::Key_F11:				return KeySym::F11;
		case Qt::Key_F12:				return KeySym::F12;
		case Qt::Key_F13:				return KeySym::F13;
		case Qt::Key_F14:				return KeySym::F14;
		case Qt::Key_F15:				return KeySym::F15;

		case Qt::Key_NumLock:			return KeySym::NUM_LOCK;
		case Qt::Key_CapsLock:			return KeySym::CAPS_LOCK;
		case Qt::Key_ScrollLock:		return KeySym::SCROLL_LOCK;
		//case Qt::Key_Shift:				return KeySym::RSHIFT;
		case Qt::Key_Shift:				return KeySym::LSHIFT;
		//case Qt::Key_Control:			return KeySym::RCTRL;
		case Qt::Key_Control:			return KeySym::LCTRL;
		case Qt::Key_AltGr:				return KeySym::RALT;
		case Qt::Key_Alt:				return KeySym::LALT;
		case Qt::Key_Super_R:			return KeySym::RSUPER;
		case Qt::Key_Super_L:			return KeySym::LSUPER;
		case Qt::Key_Mode_switch:		return KeySym::MODE;
		case Qt::Key_ApplicationLeft:	return KeySym::APPLICATION;

		case Qt::Key_Help:				return KeySym::HELP;
		case Qt::Key_Print:				return KeySym::PRINTSCREEN;
		case Qt::Key_SysReq:			return KeySym::SYSREQ;
		case Qt::Key_Menu:				return KeySym::MENU;
		case Qt::Key_PowerOff:			return KeySym::POWER;
		case Qt::Key_Undo:				return KeySym::UNDO;

		default:					return KeySym::UNKNOWN;
	}
	// clang-format on
}

int Qt5Keys::keyModMaskToEnumMask(Qt::KeyboardModifiers keymod)
{
	int result = 0;

	if (keymod != Qt::NoModifier)
	{
		result |= (keymod & Qt::ShiftModifier) ? KeyMod::LSHIFT : 0;
		result |= (keymod & Qt::ShiftModifier) ? KeyMod::RSHIFT : 0;
		result |= (keymod & Qt::ControlModifier) ? KeyMod::LCTRL : 0;
		result |= (keymod & Qt::ControlModifier) ? KeyMod::RCTRL : 0;
		result |= (keymod & Qt::AltModifier) ? KeyMod::LALT : 0;
		result |= (keymod & Qt::AltModifier) ? KeyMod::RALT : 0;
		result |= (keymod & Qt::MetaModifier) ? KeyMod::LSUPER : 0;
		result |= (keymod & Qt::MetaModifier) ? KeyMod::RSUPER : 0;
		result |= (keymod & Qt::KeypadModifier) ? KeyMod::NUM : 0;
		//result |= (keymod & KMOD_CAPS) ? KeyMod::CAPS : 0;
		result |= (keymod & Qt::GroupSwitchModifier) ? KeyMod::MODE : 0;
	}

	return result;
}

/*

int SdlKeys::enumToKeySymValue(KeySym keysym)
{
	// clang-format off
	switch (keysym)
	{
		case KeySym::UNKNOWN:			return Qt::Key_UNKNOWN;
		case KeySym::BACKSPACE:			return Qt::Key_BACKSPACE;
		case KeySym::TAB:				return Qt::Key_TAB;
		case KeySym::CLEAR:				return Qt::Key_CLEAR;
		case KeySym::RETURN:			return Qt::Key_RETURN;
		case KeySym::PAUSE:				return Qt::Key_PAUSE;
		case KeySym::ESCAPE:			return Qt::Key_ESCAPE;
		case KeySym::SPACE:				return Qt::Key_SPACE;
		case KeySym::EXCLAIM:			return Qt::Key_EXCLAIM;
		case KeySym::QUOTEDBL:			return Qt::Key_QUOTEDBL;
		case KeySym::HASH:				return Qt::Key_HASH;
		case KeySym::DOLLAR:			return Qt::Key_DOLLAR;
		case KeySym::AMPERSAND:			return Qt::Key_AMPERSAND;
		case KeySym::QUOTE:				return Qt::Key_QUOTE;
		case KeySym::LEFTPAREN:			return Qt::Key_LEFTPAREN;
		case KeySym::RIGHTPAREN:		return Qt::Key_RIGHTPAREN;
		case KeySym::ASTERISK:			return Qt::Key_ASTERISK;
		case KeySym::PLUS:				return Qt::Key_PLUS;
		case KeySym::COMMA:				return Qt::Key_COMMA;
		case KeySym::MINUS:				return Qt::Key_MINUS;
		case KeySym::PERIOD:			return Qt::Key_PERIOD;
		case KeySym::SLASH:				return Qt::Key_SLASH;
		case KeySym::N0:				return Qt::Key_0;
		case KeySym::N1:				return Qt::Key_1;
		case KeySym::N2:				return Qt::Key_2;
		case KeySym::N3:				return Qt::Key_3;
		case KeySym::N4:				return Qt::Key_4;
		case KeySym::N5:				return Qt::Key_5;
		case KeySym::N6:				return Qt::Key_6;
		case KeySym::N7:				return Qt::Key_7;
		case KeySym::N8:				return Qt::Key_8;
		case KeySym::N9:				return Qt::Key_9;
		case KeySym::COLON:				return Qt::Key_COLON;
		case KeySym::SEMICOLON:			return Qt::Key_SEMICOLON;
		case KeySym::LESS:				return Qt::Key_LESS;
		case KeySym::EQUALS:			return Qt::Key_EQUALS;
		case KeySym::GREATER:			return Qt::Key_GREATER;
		case KeySym::QUESTION:			return Qt::Key_QUESTION;
		case KeySym::AT:				return Qt::Key_AT;

		case KeySym::LEFTBRACKET:		return Qt::Key_LEFTBRACKET;
		case KeySym::BACKSLASH:			return Qt::Key_BACKSLASH;
		case KeySym::RIGHTBRACKET:		return Qt::Key_RIGHTBRACKET;
		case KeySym::CARET:				return Qt::Key_CARET;
		case KeySym::UNDERSCORE:		return Qt::Key_UNDERSCORE;
		case KeySym::BACKQUOTE:			return Qt::Key_BACKQUOTE;
		case KeySym::A:					return Qt::Key_a;
		case KeySym::B:					return Qt::Key_b;
		case KeySym::C:					return Qt::Key_c;
		case KeySym::D:					return Qt::Key_d;
		case KeySym::E:					return Qt::Key_e;
		case KeySym::F:					return Qt::Key_f;
		case KeySym::G:					return Qt::Key_g;
		case KeySym::H:					return Qt::Key_h;
		case KeySym::I:					return Qt::Key_i;
		case KeySym::J:					return Qt::Key_j;
		case KeySym::K:					return Qt::Key_k;
		case KeySym::L:					return Qt::Key_l;
		case KeySym::M:					return Qt::Key_m;
		case KeySym::N:					return Qt::Key_n;
		case KeySym::O:					return Qt::Key_o;
		case KeySym::P:					return Qt::Key_p;
		case KeySym::Q:					return Qt::Key_q;
		case KeySym::R:					return Qt::Key_r;
		case KeySym::S:					return Qt::Key_s;
		case KeySym::T:					return Qt::Key_t;
		case KeySym::U:					return Qt::Key_u;
		case KeySym::V:					return Qt::Key_v;
		case KeySym::W:					return Qt::Key_w;
		case KeySym::X:					return Qt::Key_x;
		case KeySym::Y:					return Qt::Key_y;
		case KeySym::Z:					return Qt::Key_z;
		case KeySym::DELETE:			return Qt::Key_DELETE;

		case KeySym::KP0:				return Qt::Key_KP_0;
		case KeySym::KP1:				return Qt::Key_KP_1;
		case KeySym::KP2:				return Qt::Key_KP_2;
		case KeySym::KP3:				return Qt::Key_KP_3;
		case KeySym::KP4:				return Qt::Key_KP_4;
		case KeySym::KP5:				return Qt::Key_KP_5;
		case KeySym::KP6:				return Qt::Key_KP_6;
		case KeySym::KP7:				return Qt::Key_KP_7;
		case KeySym::KP8:				return Qt::Key_KP_8;
		case KeySym::KP9:				return Qt::Key_KP_9;
		case KeySym::KP_PERIOD:			return Qt::Key_KP_PERIOD;
		case KeySym::KP_DIVIDE:			return Qt::Key_KP_DIVIDE;
		case KeySym::KP_MULTIPLY:		return Qt::Key_KP_MULTIPLY;
		case KeySym::KP_MINUS:			return Qt::Key_KP_MINUS;
		case KeySym::KP_PLUS:			return Qt::Key_KP_PLUS;
		case KeySym::KP_ENTER:			return Qt::Key_KP_ENTER;
		case KeySym::KP_EQUALS:			return Qt::Key_KP_EQUALS;

		case KeySym::UP:				return Qt::Key_UP;
		case KeySym::DOWN:				return Qt::Key_DOWN;
		case KeySym::RIGHT:				return Qt::Key_RIGHT;
		case KeySym::LEFT:				return Qt::Key_LEFT;
		case KeySym::INSERT:			return Qt::Key_INSERT;
		case KeySym::HOME:				return Qt::Key_HOME;
		case KeySym::END:				return Qt::Key_END;
		case KeySym::PAGEUP:			return Qt::Key_PAGEUP;
		case KeySym::PAGEDOWN:			return Qt::Key_PAGEDOWN;

		case KeySym::F1:				return Qt::Key_F1;
		case KeySym::F2:				return Qt::Key_F2;
		case KeySym::F3:				return Qt::Key_F3;
		case KeySym::F4:				return Qt::Key_F4;
		case KeySym::F5:				return Qt::Key_F5;
		case KeySym::F6:				return Qt::Key_F6;
		case KeySym::F7:				return Qt::Key_F7;
		case KeySym::F8:				return Qt::Key_F8;
		case KeySym::F9:				return Qt::Key_F9;
		case KeySym::F10:				return Qt::Key_F10;
		case KeySym::F11:				return Qt::Key_F11;
		case KeySym::F12:				return Qt::Key_F12;
		case KeySym::F13:				return Qt::Key_F13;
		case KeySym::F14:				return Qt::Key_F14;
		case KeySym::F15:				return Qt::Key_F15;

		case KeySym::NUM_LOCK:			return Qt::Key_NUMLOCKCLEAR;
		case KeySym::CAPS_LOCK:			return Qt::Key_CAPSLOCK;
		case KeySym::SCROLL_LOCK:		return Qt::Key_SCROLLLOCK;
		case KeySym::RSHIFT:			return Qt::Key_RSHIFT;
		case KeySym::LSHIFT:			return Qt::Key_LSHIFT;
		case KeySym::RCTRL:				return Qt::Key_RCTRL;
		case KeySym::LCTRL:				return Qt::Key_LCTRL;
		case KeySym::RALT:				return Qt::Key_RALT;
		case KeySym::LALT:				return Qt::Key_LALT;
		case KeySym::RSUPER:			return Qt::Key_RGUI;
		case KeySym::LSUPER:			return Qt::Key_LGUI;
		case KeySym::MODE:				return Qt::Key_MODE;
		case KeySym::APPLICATION:		return Qt::Key_APPLICATION;

		case KeySym::HELP:				return Qt::Key_HELP;
		case KeySym::PRINTSCREEN:		return Qt::Key_PRINTSCREEN;
		case KeySym::SYSREQ:			return Qt::Key_SYSREQ;
		case KeySym::MENU:				return Qt::Key_MENU;
		case KeySym::POWER:				return Qt::Key_POWER;
		case KeySym::UNDO:				return Qt::Key_UNDO;

		default:						return Qt::Key_UNKNOWN;
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
*/

}
