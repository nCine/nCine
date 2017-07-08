#include "AndroidInputManager.h"
#include <android/input.h>

namespace ncine {

KeySym AndroidKeys::keySymValueToEnum(int keysym)
{
	switch (keysym)
	{
		case AKEYCODE_UNKNOWN:			return KEY_UNKNOWN;
		case AKEYCODE_SOFT_LEFT:		return KEY_SOFT_LEFT;
		case AKEYCODE_SOFT_RIGHT:		return KEY_SOFT_RIGHT;
		case AKEYCODE_HOME:				return KEY_HOME;
		case AKEYCODE_BACK:				return KEY_BACK;
		case AKEYCODE_CALL:				return KEY_CALL;
		case AKEYCODE_ENDCALL:			return KEY_ENDCALL;
		case AKEYCODE_0:				return KEY_0;
		case AKEYCODE_1:				return KEY_1;
		case AKEYCODE_2:				return KEY_2;
		case AKEYCODE_3:				return KEY_3;
		case AKEYCODE_4:				return KEY_4;
		case AKEYCODE_5:				return KEY_5;
		case AKEYCODE_6:				return KEY_6;
		case AKEYCODE_7:				return KEY_7;
		case AKEYCODE_8:				return KEY_8;
		case AKEYCODE_9:				return KEY_9;
		case AKEYCODE_STAR:				return KEY_STAR;
		case AKEYCODE_POUND:			return KEY_POUND;
		case AKEYCODE_DPAD_UP:			return KEY_UP;
		case AKEYCODE_DPAD_DOWN:		return KEY_DOWN;
		case AKEYCODE_DPAD_LEFT:		return KEY_LEFT;
		case AKEYCODE_DPAD_RIGHT:		return KEY_RIGHT;
		case AKEYCODE_DPAD_CENTER:		return KEY_DPAD_CENTER;
		case AKEYCODE_VOLUME_UP:		return KEY_VOLUME_UP;
		case AKEYCODE_VOLUME_DOWN:		return KEY_VOLUME_DOWN;
		case AKEYCODE_POWER:			return KEY_POWER;
		case AKEYCODE_CAMERA:			return KEY_CAMERA;
		case AKEYCODE_CLEAR:			return KEY_CLEAR;
		case AKEYCODE_A:				return KEY_A;
		case AKEYCODE_B:				return KEY_B;
		case AKEYCODE_C:				return KEY_C;
		case AKEYCODE_D:				return KEY_D;
		case AKEYCODE_E:				return KEY_E;
		case AKEYCODE_F:				return KEY_F;
		case AKEYCODE_G:				return KEY_G;
		case AKEYCODE_H:				return KEY_H;
		case AKEYCODE_I:				return KEY_I;
		case AKEYCODE_J:				return KEY_J;
		case AKEYCODE_K:				return KEY_K;
		case AKEYCODE_L:				return KEY_L;
		case AKEYCODE_M:				return KEY_M;
		case AKEYCODE_N:				return KEY_N;
		case AKEYCODE_O:				return KEY_O;
		case AKEYCODE_P:				return KEY_P;
		case AKEYCODE_Q:				return KEY_Q;
		case AKEYCODE_R:				return KEY_R;
		case AKEYCODE_S:				return KEY_S;
		case AKEYCODE_T:				return KEY_T;
		case AKEYCODE_U:				return KEY_U;
		case AKEYCODE_V:				return KEY_V;
		case AKEYCODE_W:				return KEY_W;
		case AKEYCODE_X:				return KEY_X;
		case AKEYCODE_Y:				return KEY_Y;
		case AKEYCODE_Z:				return KEY_Z;
		case AKEYCODE_COMMA:			return KEY_COMMA;
		case AKEYCODE_PERIOD:			return KEY_PERIOD;
		case AKEYCODE_ALT_LEFT:			return KEY_LALT;
		case AKEYCODE_ALT_RIGHT:		return KEY_RALT;
		case AKEYCODE_SHIFT_LEFT:		return KEY_LSHIFT;
		case AKEYCODE_SHIFT_RIGHT:		return KEY_RSHIFT;
		case AKEYCODE_TAB:				return KEY_TAB;
		case AKEYCODE_SPACE:			return KEY_SPACE;
		case AKEYCODE_SYM:				return KEY_SYM;
		case AKEYCODE_EXPLORER:			return KEY_EXPLORER;
		case AKEYCODE_ENVELOPE:			return KEY_ENVELOPE;
		case AKEYCODE_ENTER:			return KEY_RETURN;
		case AKEYCODE_DEL:				return KEY_DELETE;
		case AKEYCODE_GRAVE:			return KEY_BACKQUOTE;
		case AKEYCODE_MINUS:			return KEY_MINUS;
		case AKEYCODE_EQUALS:			return KEY_EQUALS;
		case AKEYCODE_LEFT_BRACKET:		return KEY_LEFTBRACKET;
		case AKEYCODE_RIGHT_BRACKET:	return KEY_RIGHTBRACKET;
		case AKEYCODE_BACKSLASH:		return KEY_BACKSLASH;
		case AKEYCODE_SEMICOLON:		return KEY_SEMICOLON;
		case AKEYCODE_APOSTROPHE:		return KEY_QUOTE;
		case AKEYCODE_SLASH:			return KEY_SLASH;
		case AKEYCODE_AT:				return KEY_AT;
		case AKEYCODE_NUM:				return KEY_NUM;
		case AKEYCODE_HEADSETHOOK:		return KEY_HEADSETHOOK;
		case AKEYCODE_FOCUS:			return KEY_FOCUS; // *camera* focus
		case AKEYCODE_PLUS:				return KEY_PLUS;
		case AKEYCODE_MENU:				return KEY_MENU;
		case AKEYCODE_NOTIFICATION:		return KEY_NOTIFICATION;
		case AKEYCODE_SEARCH:			return KEY_SEARCH;
		case AKEYCODE_MEDIA_PLAY_PAUSE:	return KEY_MEDIA_PLAY_PAUSE;
		case AKEYCODE_MEDIA_STOP:		return KEY_MEDIA_STOP;
		case AKEYCODE_MEDIA_NEXT:		return KEY_MEDIA_NEXT;
		case AKEYCODE_MEDIA_PREVIOUS:	return KEY_MEDIA_PREVIOUS;
		case AKEYCODE_MEDIA_REWIND:		return KEY_MEDIA_REWIND;
		case AKEYCODE_MEDIA_FAST_FORWARD:	return KEY_MEDIA_FAST_FORWARD;
		case AKEYCODE_MUTE:				return KEY_MUTE;
		case AKEYCODE_PAGE_UP:			return KEY_PAGEUP;
		case AKEYCODE_PAGE_DOWN:		return KEY_PAGEDOWN;
		case AKEYCODE_PICTSYMBOLS:		return KEY_PICTSYMBOLS;
		case AKEYCODE_SWITCH_CHARSET:	return KEY_SWITCH_CHARSET;
		case AKEYCODE_BUTTON_A:			return KEY_BUTTON_A;
		case AKEYCODE_BUTTON_B:			return KEY_BUTTON_B;
		case AKEYCODE_BUTTON_C:			return KEY_BUTTON_C;
		case AKEYCODE_BUTTON_X:			return KEY_BUTTON_X;
		case AKEYCODE_BUTTON_Y:			return KEY_BUTTON_Y;
		case AKEYCODE_BUTTON_Z:			return KEY_BUTTON_Z;
		case AKEYCODE_BUTTON_L1:		return KEY_BUTTON_L1;
		case AKEYCODE_BUTTON_R1:		return KEY_BUTTON_R1;
		case AKEYCODE_BUTTON_L2:		return KEY_BUTTON_L2;
		case AKEYCODE_BUTTON_R2:		return KEY_BUTTON_R2;
		case AKEYCODE_BUTTON_THUMBL:	return KEY_BUTTON_THUMBL;
		case AKEYCODE_BUTTON_THUMBR:	return KEY_BUTTON_THUMBR;
		case AKEYCODE_BUTTON_START:		return KEY_BUTTON_START;
		case AKEYCODE_BUTTON_SELECT:	return KEY_BUTTON_SELECT;
		case AKEYCODE_BUTTON_MODE:		return KEY_BUTTON_MODE;
	#if __ANDROID_API__ >= 13
		case AKEYCODE_ESCAPE:			return KEY_ESCAPE;
		case AKEYCODE_FORWARD_DEL:		return KEY_FORWARD_DEL;
		case AKEYCODE_CTRL_LEFT:		return KEY_LCTRL;
		case AKEYCODE_CTRL_RIGHT:		return KEY_RCTRL;
		case AKEYCODE_CAPS_LOCK:		return KEY_CAPS_LOCK;
		case AKEYCODE_SCROLL_LOCK:		return KEY_SCROLL_LOCK;
		case AKEYCODE_META_LEFT:		return KEY_LSUPER;
		case AKEYCODE_META_RIGHT:		return KEY_RSUPER;
		case AKEYCODE_FUNCTION:			return KEY_FUNCTION;
		case AKEYCODE_SYSRQ:			return KEY_SYSREQ;
		case AKEYCODE_BREAK:			return KEY_PAUSE;
		case AKEYCODE_MOVE_HOME:		return KEY_MOVE_HOME;
		case AKEYCODE_MOVE_END:			return KEY_MOVE_END;
		case AKEYCODE_INSERT:			return KEY_INSERT;
		case AKEYCODE_FORWARD:			return KEY_FORWARD;
		case AKEYCODE_MEDIA_PLAY:		return KEY_MEDIA_PLAY;
		case AKEYCODE_MEDIA_PAUSE:		return KEY_MEDIA_PAUSE;
		case AKEYCODE_MEDIA_CLOSE:		return KEY_MEDIA_CLOSE;
		case AKEYCODE_MEDIA_EJECT:		return KEY_MEDIA_EJECT;
		case AKEYCODE_MEDIA_RECORD:		return KEY_MEDIA_EJECT;
		case AKEYCODE_F1:				return KEY_F1;
		case AKEYCODE_F2:				return KEY_F2;
		case AKEYCODE_F3:				return KEY_F3;
		case AKEYCODE_F4:				return KEY_F4;
		case AKEYCODE_F5:				return KEY_F5;
		case AKEYCODE_F6:				return KEY_F6;
		case AKEYCODE_F7:				return KEY_F7;
		case AKEYCODE_F8:				return KEY_F8;
		case AKEYCODE_F9:				return KEY_F9;
		case AKEYCODE_F10:				return KEY_F10;
		case AKEYCODE_F11:				return KEY_F11;
		case AKEYCODE_F12:				return KEY_F12;
		case AKEYCODE_NUM_LOCK:			return KEY_NUM_LOCK;
		case AKEYCODE_NUMPAD_0:			return KEY_KP0;
		case AKEYCODE_NUMPAD_1:			return KEY_KP1;
		case AKEYCODE_NUMPAD_2:			return KEY_KP2;
		case AKEYCODE_NUMPAD_3:			return KEY_KP3;
		case AKEYCODE_NUMPAD_4:			return KEY_KP4;
		case AKEYCODE_NUMPAD_5:			return KEY_KP5;
		case AKEYCODE_NUMPAD_6:			return KEY_KP6;
		case AKEYCODE_NUMPAD_7:			return KEY_KP7;
		case AKEYCODE_NUMPAD_8:			return KEY_KP8;
		case AKEYCODE_NUMPAD_9:			return KEY_KP9;
		case AKEYCODE_NUMPAD_DIVIDE:	return KEY_KP_DIVIDE;
		case AKEYCODE_NUMPAD_MULTIPLY:	return KEY_KP_MULTIPLY;
		case AKEYCODE_NUMPAD_SUBTRACT:	return KEY_KP_MINUS;
		case AKEYCODE_NUMPAD_ADD:		return KEY_KP_PLUS;
		case AKEYCODE_NUMPAD_DOT:		return KEY_KP_PERIOD;
		case AKEYCODE_NUMPAD_COMMA:		return KEY_KP_COMMA;
		case AKEYCODE_NUMPAD_ENTER:		return KEY_KP_ENTER;
		case AKEYCODE_NUMPAD_EQUALS:	return KEY_KP_EQUALS;
		case AKEYCODE_NUMPAD_LEFT_PAREN:	return KEY_KP_LEFTPAREN;
		case AKEYCODE_NUMPAD_RIGHT_PAREN:	return KEY_KP_RIGHTPAREN;
		case AKEYCODE_VOLUME_MUTE:		return KEY_VOLUME_MUTE;
		case AKEYCODE_INFO:				return KEY_INFO;
		case AKEYCODE_CHANNEL_UP:		return KEY_CHANNEL_UP;
		case AKEYCODE_CHANNEL_DOWN:		return KEY_CHANNEL_DOWN;
		case AKEYCODE_ZOOM_IN:			return KEY_ZOOM_IN;
		case AKEYCODE_ZOOM_OUT:			return KEY_ZOOM_OUT;
		case AKEYCODE_TV:				return KEY_TV;
		case AKEYCODE_WINDOW:			return KEY_WINDOW;
		case AKEYCODE_GUIDE:			return KEY_GUIDE;
		case AKEYCODE_DVR:				return KEY_DVR;
		case AKEYCODE_BOOKMARK:			return KEY_BOOKMARK;
		case AKEYCODE_CAPTIONS:			return KEY_CAPTIONS;
		case AKEYCODE_SETTINGS:			return KEY_SETTINGS;
		case AKEYCODE_TV_POWER:			return KEY_TV_POWER;
		case AKEYCODE_TV_INPUT:			return KEY_TV_INPUT;
		case AKEYCODE_STB_POWER:		return KEY_STB_POWER;
		case AKEYCODE_STB_INPUT:		return KEY_STB_INPUT;
		case AKEYCODE_AVR_POWER:		return KEY_AVR_POWER;
		case AKEYCODE_AVR_INPUT:		return KEY_AVR_INPUT;
		case AKEYCODE_PROG_RED:			return KEY_PROG_RED;
		case AKEYCODE_PROG_GREEN:		return KEY_PROG_GREEN;
		case AKEYCODE_PROG_YELLOW:		return KEY_PROG_YELLOW;
		case AKEYCODE_PROG_BLUE:		return KEY_PROG_BLUE;
		case AKEYCODE_APP_SWITCH:		return KEY_APP_SWITCH;
		case AKEYCODE_BUTTON_1:			return KEY_BUTTON_1;
		case AKEYCODE_BUTTON_2:			return KEY_BUTTON_2;
		case AKEYCODE_BUTTON_3:			return KEY_BUTTON_3;
		case AKEYCODE_BUTTON_4:			return KEY_BUTTON_4;
		case AKEYCODE_BUTTON_5:			return KEY_BUTTON_5;
		case AKEYCODE_BUTTON_6:			return KEY_BUTTON_6;
		case AKEYCODE_BUTTON_7:			return KEY_BUTTON_7;
		case AKEYCODE_BUTTON_8:			return KEY_BUTTON_8;
		case AKEYCODE_BUTTON_9:			return KEY_BUTTON_9;
		case AKEYCODE_BUTTON_10:		return KEY_BUTTON_10;
		case AKEYCODE_BUTTON_11:		return KEY_BUTTON_11;
		case AKEYCODE_BUTTON_12:		return KEY_BUTTON_12;
		case AKEYCODE_BUTTON_13:		return KEY_BUTTON_13;
		case AKEYCODE_BUTTON_14:		return KEY_BUTTON_14;
		case AKEYCODE_BUTTON_15:		return KEY_BUTTON_15;
		case AKEYCODE_BUTTON_16:		return KEY_BUTTON_16;
	#endif

		default:						return KEY_UNKNOWN;
	}
}

KeyMod AndroidKeys::keyModValueToEnum(int keymod)
{
	switch (keymod)
	{
		case AMETA_NONE:				return KEYMOD_NONE;
		case AMETA_ALT_ON:				return KEYMOD_ALT;
		case AMETA_ALT_LEFT_ON:			return KEYMOD_LALT;
		case AMETA_ALT_RIGHT_ON:		return KEYMOD_RALT;
		case AMETA_SHIFT_ON:			return KEYMOD_SHIFT;
		case AMETA_SHIFT_LEFT_ON:		return KEYMOD_LSHIFT;
		case AMETA_SHIFT_RIGHT_ON:		return KEYMOD_RSHIFT;
#if __ANDROID_API__ >= 13
		case AMETA_CTRL_ON:				return KEYMOD_CTRL;
		case AMETA_CTRL_LEFT_ON:		return KEYMOD_LCTRL;
		case AMETA_CTRL_RIGHT_ON:		return KEYMOD_RCTRL;
#endif
		case AMETA_META_ON:				return KEYMOD_SUPER;
		case AMETA_META_LEFT_ON:		return KEYMOD_LSUPER;
		case AMETA_META_RIGHT_ON:		return KEYMOD_RSUPER;
#if __ANDROID_API__ >= 13
		case AMETA_CAPS_LOCK_ON:		return KEYMOD_CAPS;
		case AMETA_NUM_LOCK_ON:			return KEYMOD_NUM;
		case AMETA_SCROLL_LOCK_ON:		return KEYMOD_SCROLL;
#endif
		case AMETA_SYM_ON:				return KEYMOD_SYM;

		default:						return KEYMOD_NONE;
	}
}

}
