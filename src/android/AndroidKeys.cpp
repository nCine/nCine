#include "AndroidInputManager.h"
#include <android/input.h>

namespace ncine {

KeySym AndroidKeys::keySymValueToEnum(int keysym)
{
	// clang-format off
	switch (keysym)
	{
		case AKEYCODE_UNKNOWN:				return KeySym::UNKNOWN;
		case AKEYCODE_SOFT_LEFT:			return KeySym::SOFT_LEFT;
		case AKEYCODE_SOFT_RIGHT:			return KeySym::SOFT_RIGHT;
		case AKEYCODE_HOME:					return KeySym::HOME;
		case AKEYCODE_BACK:					return KeySym::BACK;
		case AKEYCODE_CALL:					return KeySym::CALL;
		case AKEYCODE_ENDCALL:				return KeySym::ENDCALL;
		case AKEYCODE_0:					return KeySym::N0;
		case AKEYCODE_1:					return KeySym::N1;
		case AKEYCODE_2:					return KeySym::N2;
		case AKEYCODE_3:					return KeySym::N3;
		case AKEYCODE_4:					return KeySym::N4;
		case AKEYCODE_5:					return KeySym::N5;
		case AKEYCODE_6:					return KeySym::N6;
		case AKEYCODE_7:					return KeySym::N7;
		case AKEYCODE_8:					return KeySym::N8;
		case AKEYCODE_9:					return KeySym::N9;
		case AKEYCODE_STAR:					return KeySym::STAR;
		case AKEYCODE_POUND:				return KeySym::POUND;
		case AKEYCODE_DPAD_UP:				return KeySym::UP;
		case AKEYCODE_DPAD_DOWN:			return KeySym::DOWN;
		case AKEYCODE_DPAD_LEFT:			return KeySym::LEFT;
		case AKEYCODE_DPAD_RIGHT:			return KeySym::RIGHT;
		case AKEYCODE_DPAD_CENTER:			return KeySym::DPAD_CENTER;
		case AKEYCODE_VOLUME_UP:			return KeySym::VOLUME_UP;
		case AKEYCODE_VOLUME_DOWN:			return KeySym::VOLUME_DOWN;
		case AKEYCODE_POWER:				return KeySym::POWER;
		case AKEYCODE_CAMERA:				return KeySym::CAMERA;
		case AKEYCODE_CLEAR:				return KeySym::CLEAR;
		case AKEYCODE_A:					return KeySym::A;
		case AKEYCODE_B:					return KeySym::B;
		case AKEYCODE_C:					return KeySym::C;
		case AKEYCODE_D:					return KeySym::D;
		case AKEYCODE_E:					return KeySym::E;
		case AKEYCODE_F:					return KeySym::F;
		case AKEYCODE_G:					return KeySym::G;
		case AKEYCODE_H:					return KeySym::H;
		case AKEYCODE_I:					return KeySym::I;
		case AKEYCODE_J:					return KeySym::J;
		case AKEYCODE_K:					return KeySym::K;
		case AKEYCODE_L:					return KeySym::L;
		case AKEYCODE_M:					return KeySym::M;
		case AKEYCODE_N:					return KeySym::N;
		case AKEYCODE_O:					return KeySym::O;
		case AKEYCODE_P:					return KeySym::P;
		case AKEYCODE_Q:					return KeySym::Q;
		case AKEYCODE_R:					return KeySym::R;
		case AKEYCODE_S:					return KeySym::S;
		case AKEYCODE_T:					return KeySym::T;
		case AKEYCODE_U:					return KeySym::U;
		case AKEYCODE_V:					return KeySym::V;
		case AKEYCODE_W:					return KeySym::W;
		case AKEYCODE_X:					return KeySym::X;
		case AKEYCODE_Y:					return KeySym::Y;
		case AKEYCODE_Z:					return KeySym::Z;
		case AKEYCODE_COMMA:				return KeySym::COMMA;
		case AKEYCODE_PERIOD:				return KeySym::PERIOD;
		case AKEYCODE_ALT_LEFT:				return KeySym::LALT;
		case AKEYCODE_ALT_RIGHT:			return KeySym::RALT;
		case AKEYCODE_SHIFT_LEFT:			return KeySym::LSHIFT;
		case AKEYCODE_SHIFT_RIGHT:			return KeySym::RSHIFT;
		case AKEYCODE_TAB:					return KeySym::TAB;
		case AKEYCODE_SPACE:				return KeySym::SPACE;
		case AKEYCODE_SYM:					return KeySym::SYM;
		case AKEYCODE_EXPLORER:				return KeySym::EXPLORER;
		case AKEYCODE_ENVELOPE:				return KeySym::ENVELOPE;
		case AKEYCODE_ENTER:				return KeySym::RETURN;
		case AKEYCODE_DEL:					return KeySym::BACKSPACE;
		case AKEYCODE_GRAVE:				return KeySym::BACKQUOTE;
		case AKEYCODE_MINUS:				return KeySym::MINUS;
		case AKEYCODE_EQUALS:				return KeySym::EQUALS;
		case AKEYCODE_LEFT_BRACKET:			return KeySym::LEFTBRACKET;
		case AKEYCODE_RIGHT_BRACKET:		return KeySym::RIGHTBRACKET;
		case AKEYCODE_BACKSLASH:			return KeySym::BACKSLASH;
		case AKEYCODE_SEMICOLON:			return KeySym::SEMICOLON;
		case AKEYCODE_APOSTROPHE:			return KeySym::QUOTE;
		case AKEYCODE_SLASH:				return KeySym::SLASH;
		case AKEYCODE_AT:					return KeySym::AT;
		case AKEYCODE_NUM:					return KeySym::NUM;
		case AKEYCODE_HEADSETHOOK:			return KeySym::HEADSETHOOK;
		case AKEYCODE_FOCUS:				return KeySym::FOCUS; // *camera* focus
		case AKEYCODE_PLUS:					return KeySym::PLUS;
		case AKEYCODE_MENU:					return KeySym::MENU;
		case AKEYCODE_NOTIFICATION:			return KeySym::NOTIFICATION;
		case AKEYCODE_SEARCH:				return KeySym::SEARCH;
		case AKEYCODE_MEDIA_PLAY_PAUSE:		return KeySym::MEDIA_PLAY_PAUSE;
		case AKEYCODE_MEDIA_STOP:			return KeySym::MEDIA_STOP;
		case AKEYCODE_MEDIA_NEXT:			return KeySym::MEDIA_NEXT;
		case AKEYCODE_MEDIA_PREVIOUS:		return KeySym::MEDIA_PREVIOUS;
		case AKEYCODE_MEDIA_REWIND:			return KeySym::MEDIA_REWIND;
		case AKEYCODE_MEDIA_FAST_FORWARD:	return KeySym::MEDIA_FAST_FORWARD;
		case AKEYCODE_MUTE:					return KeySym::MUTE;
		case AKEYCODE_PAGE_UP:				return KeySym::PAGEUP;
		case AKEYCODE_PAGE_DOWN:			return KeySym::PAGEDOWN;
		case AKEYCODE_PICTSYMBOLS:			return KeySym::PICTSYMBOLS;
		case AKEYCODE_SWITCH_CHARSET:		return KeySym::SWITCH_CHARSET;
		case AKEYCODE_BUTTON_A:				return KeySym::BUTTON_A;
		case AKEYCODE_BUTTON_B:				return KeySym::BUTTON_B;
		case AKEYCODE_BUTTON_C:				return KeySym::BUTTON_C;
		case AKEYCODE_BUTTON_X:				return KeySym::BUTTON_X;
		case AKEYCODE_BUTTON_Y:				return KeySym::BUTTON_Y;
		case AKEYCODE_BUTTON_Z:				return KeySym::BUTTON_Z;
		case AKEYCODE_BUTTON_L1:			return KeySym::BUTTON_L1;
		case AKEYCODE_BUTTON_R1:			return KeySym::BUTTON_R1;
		case AKEYCODE_BUTTON_L2:			return KeySym::BUTTON_L2;
		case AKEYCODE_BUTTON_R2:			return KeySym::BUTTON_R2;
		case AKEYCODE_BUTTON_THUMBL:		return KeySym::BUTTON_THUMBL;
		case AKEYCODE_BUTTON_THUMBR:		return KeySym::BUTTON_THUMBR;
		case AKEYCODE_BUTTON_START:			return KeySym::BUTTON_START;
		case AKEYCODE_BUTTON_SELECT:		return KeySym::BUTTON_SELECT;
		case AKEYCODE_BUTTON_MODE:			return KeySym::BUTTON_MODE;
#if __ANDROID_API__ >= 13
		case AKEYCODE_ESCAPE:				return KeySym::ESCAPE;
		case AKEYCODE_FORWARD_DEL:			return KeySym::DELETE;
		case AKEYCODE_CTRL_LEFT:			return KeySym::LCTRL;
		case AKEYCODE_CTRL_RIGHT:			return KeySym::RCTRL;
		case AKEYCODE_CAPS_LOCK:			return KeySym::CAPS_LOCK;
		case AKEYCODE_SCROLL_LOCK:			return KeySym::SCROLL_LOCK;
		case AKEYCODE_META_LEFT:			return KeySym::LSUPER;
		case AKEYCODE_META_RIGHT:			return KeySym::RSUPER;
		case AKEYCODE_FUNCTION:				return KeySym::FUNCTION_KEY;
		case AKEYCODE_SYSRQ:				return KeySym::SYSREQ;
		case AKEYCODE_BREAK:				return KeySym::PAUSE;
		case AKEYCODE_MOVE_HOME:			return KeySym::MOVE_HOME;
		case AKEYCODE_MOVE_END:				return KeySym::MOVE_END;
		case AKEYCODE_INSERT:				return KeySym::INSERT;
		case AKEYCODE_FORWARD:				return KeySym::FORWARD;
		case AKEYCODE_MEDIA_PLAY:			return KeySym::MEDIA_PLAY;
		case AKEYCODE_MEDIA_PAUSE:			return KeySym::MEDIA_PAUSE;
		case AKEYCODE_MEDIA_CLOSE:			return KeySym::MEDIA_CLOSE;
		case AKEYCODE_MEDIA_EJECT:			return KeySym::MEDIA_EJECT;
		case AKEYCODE_MEDIA_RECORD:			return KeySym::MEDIA_EJECT;
		case AKEYCODE_F1:					return KeySym::F1;
		case AKEYCODE_F2:					return KeySym::F2;
		case AKEYCODE_F3:					return KeySym::F3;
		case AKEYCODE_F4:					return KeySym::F4;
		case AKEYCODE_F5:					return KeySym::F5;
		case AKEYCODE_F6:					return KeySym::F6;
		case AKEYCODE_F7:					return KeySym::F7;
		case AKEYCODE_F8:					return KeySym::F8;
		case AKEYCODE_F9:					return KeySym::F9;
		case AKEYCODE_F10:					return KeySym::F10;
		case AKEYCODE_F11:					return KeySym::F11;
		case AKEYCODE_F12:					return KeySym::F12;
		case AKEYCODE_NUM_LOCK:				return KeySym::NUM_LOCK;
		case AKEYCODE_NUMPAD_0:				return KeySym::KP0;
		case AKEYCODE_NUMPAD_1:				return KeySym::KP1;
		case AKEYCODE_NUMPAD_2:				return KeySym::KP2;
		case AKEYCODE_NUMPAD_3:				return KeySym::KP3;
		case AKEYCODE_NUMPAD_4:				return KeySym::KP4;
		case AKEYCODE_NUMPAD_5:				return KeySym::KP5;
		case AKEYCODE_NUMPAD_6:				return KeySym::KP6;
		case AKEYCODE_NUMPAD_7:				return KeySym::KP7;
		case AKEYCODE_NUMPAD_8:				return KeySym::KP8;
		case AKEYCODE_NUMPAD_9:				return KeySym::KP9;
		case AKEYCODE_NUMPAD_DIVIDE:		return KeySym::KP_DIVIDE;
		case AKEYCODE_NUMPAD_MULTIPLY:		return KeySym::KP_MULTIPLY;
		case AKEYCODE_NUMPAD_SUBTRACT:		return KeySym::KP_MINUS;
		case AKEYCODE_NUMPAD_ADD:			return KeySym::KP_PLUS;
		case AKEYCODE_NUMPAD_DOT:			return KeySym::KP_PERIOD;
		case AKEYCODE_NUMPAD_COMMA:			return KeySym::KP_COMMA;
		case AKEYCODE_NUMPAD_ENTER:			return KeySym::KP_ENTER;
		case AKEYCODE_NUMPAD_EQUALS:		return KeySym::KP_EQUALS;
		case AKEYCODE_NUMPAD_LEFT_PAREN:	return KeySym::KP_LEFTPAREN;
		case AKEYCODE_NUMPAD_RIGHT_PAREN:	return KeySym::KP_RIGHTPAREN;
		case AKEYCODE_VOLUME_MUTE:			return KeySym::VOLUME_MUTE;
		case AKEYCODE_INFO:					return KeySym::INFO;
		case AKEYCODE_CHANNEL_UP:			return KeySym::CHANNEL_UP;
		case AKEYCODE_CHANNEL_DOWN:			return KeySym::CHANNEL_DOWN;
		case AKEYCODE_ZOOM_IN:				return KeySym::ZOOM_IN;
		case AKEYCODE_ZOOM_OUT:				return KeySym::ZOOM_OUT;
		case AKEYCODE_TV:					return KeySym::TV;
		case AKEYCODE_WINDOW:				return KeySym::WINDOW;
		case AKEYCODE_GUIDE:				return KeySym::GUIDE;
		case AKEYCODE_DVR:					return KeySym::DVR;
		case AKEYCODE_BOOKMARK:				return KeySym::BOOKMARK;
		case AKEYCODE_CAPTIONS:				return KeySym::CAPTIONS;
		case AKEYCODE_SETTINGS:				return KeySym::SETTINGS;
		case AKEYCODE_TV_POWER:				return KeySym::TV_POWER;
		case AKEYCODE_TV_INPUT:				return KeySym::TV_INPUT;
		case AKEYCODE_STB_POWER:			return KeySym::STB_POWER;
		case AKEYCODE_STB_INPUT:			return KeySym::STB_INPUT;
		case AKEYCODE_AVR_POWER:			return KeySym::AVR_POWER;
		case AKEYCODE_AVR_INPUT:			return KeySym::AVR_INPUT;
		case AKEYCODE_PROG_RED:				return KeySym::PROG_RED;
		case AKEYCODE_PROG_GREEN:			return KeySym::PROG_GREEN;
		case AKEYCODE_PROG_YELLOW:			return KeySym::PROG_YELLOW;
		case AKEYCODE_PROG_BLUE:			return KeySym::PROG_BLUE;
		case AKEYCODE_APP_SWITCH:			return KeySym::APP_SWITCH;
		case AKEYCODE_BUTTON_1:				return KeySym::BUTTON_1;
		case AKEYCODE_BUTTON_2:				return KeySym::BUTTON_2;
		case AKEYCODE_BUTTON_3:				return KeySym::BUTTON_3;
		case AKEYCODE_BUTTON_4:				return KeySym::BUTTON_4;
		case AKEYCODE_BUTTON_5:				return KeySym::BUTTON_5;
		case AKEYCODE_BUTTON_6:				return KeySym::BUTTON_6;
		case AKEYCODE_BUTTON_7:				return KeySym::BUTTON_7;
		case AKEYCODE_BUTTON_8:				return KeySym::BUTTON_8;
		case AKEYCODE_BUTTON_9:				return KeySym::BUTTON_9;
		case AKEYCODE_BUTTON_10:			return KeySym::BUTTON_10;
		case AKEYCODE_BUTTON_11:			return KeySym::BUTTON_11;
		case AKEYCODE_BUTTON_12:			return KeySym::BUTTON_12;
		case AKEYCODE_BUTTON_13:			return KeySym::BUTTON_13;
		case AKEYCODE_BUTTON_14:			return KeySym::BUTTON_14;
		case AKEYCODE_BUTTON_15:			return KeySym::BUTTON_15;
		case AKEYCODE_BUTTON_16:			return KeySym::BUTTON_16;
#endif
		default:							return KeySym::UNKNOWN;
	}
	// clang-format on
}

int AndroidKeys::keyModMaskToEnumMask(int keymod)
{
	int result = 0;

	if (keymod != 0)
	{
		result |= (keymod & AMETA_SHIFT_LEFT_ON) ? KeyMod::LSHIFT : 0;
		result |= (keymod & AMETA_SHIFT_RIGHT_ON) ? KeyMod::RSHIFT : 0;
		result |= (keymod & AMETA_CTRL_LEFT_ON) ? KeyMod::LCTRL : 0;
		result |= (keymod & AMETA_CTRL_RIGHT_ON) ? KeyMod::RCTRL : 0;
		result |= (keymod & AMETA_ALT_LEFT_ON) ? KeyMod::LALT : 0;
		result |= (keymod & AMETA_ALT_RIGHT_ON) ? KeyMod::RALT : 0;
		result |= (keymod & AMETA_META_LEFT_ON) ? KeyMod::LSUPER : 0;
		result |= (keymod & AMETA_META_RIGHT_ON) ? KeyMod::RSUPER : 0;
		result |= (keymod & AMETA_NUM_LOCK_ON) ? KeyMod::NUM : 0;
		result |= (keymod & AMETA_CAPS_LOCK_ON) ? KeyMod::CAPS : 0;
		result |= (keymod & AMETA_SYM_ON) ? KeyMod::SYM : 0;
	}

	return result;
}

}
