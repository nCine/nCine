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

}
