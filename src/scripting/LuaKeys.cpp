#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaKeys.h"
#include "LuaUtils.h"
#include "Keys.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaKeys::exposeConstants(lua_State *L)
{
	lua_createtable(L, static_cast<int>(KeySym::COUNT), 0);

	LuaUtils::pushField(L, "UNKNOWN", static_cast<int64_t>(KeySym::UNKNOWN));

	// Common keysyms
	LuaUtils::pushField(L, "BACKSPACE", static_cast<int64_t>(KeySym::BACKSPACE));
	LuaUtils::pushField(L, "TAB", static_cast<int64_t>(KeySym::TAB));
	LuaUtils::pushField(L, "RETURN", static_cast<int64_t>(KeySym::RETURN));
	LuaUtils::pushField(L, "ESCAPE", static_cast<int64_t>(KeySym::ESCAPE));
	LuaUtils::pushField(L, "SPACE", static_cast<int64_t>(KeySym::SPACE));
	LuaUtils::pushField(L, "QUOTE", static_cast<int64_t>(KeySym::QUOTE));
	LuaUtils::pushField(L, "PLUS", static_cast<int64_t>(KeySym::PLUS));
	LuaUtils::pushField(L, "COMMA", static_cast<int64_t>(KeySym::COMMA));
	LuaUtils::pushField(L, "MINUS", static_cast<int64_t>(KeySym::MINUS));
	LuaUtils::pushField(L, "PERIOD", static_cast<int64_t>(KeySym::PERIOD));
	LuaUtils::pushField(L, "SLASH", static_cast<int64_t>(KeySym::SLASH));
	LuaUtils::pushField(L, "N0", static_cast<int64_t>(KeySym::N0));
	LuaUtils::pushField(L, "N1", static_cast<int64_t>(KeySym::N1));
	LuaUtils::pushField(L, "N2", static_cast<int64_t>(KeySym::N2));
	LuaUtils::pushField(L, "N3", static_cast<int64_t>(KeySym::N3));
	LuaUtils::pushField(L, "N4", static_cast<int64_t>(KeySym::N4));
	LuaUtils::pushField(L, "N5", static_cast<int64_t>(KeySym::N5));
	LuaUtils::pushField(L, "N6", static_cast<int64_t>(KeySym::N6));
	LuaUtils::pushField(L, "N7", static_cast<int64_t>(KeySym::N7));
	LuaUtils::pushField(L, "N8", static_cast<int64_t>(KeySym::N8));
	LuaUtils::pushField(L, "N9", static_cast<int64_t>(KeySym::N9));
	LuaUtils::pushField(L, "SEMICOLON", static_cast<int64_t>(KeySym::SEMICOLON));
	LuaUtils::pushField(L, "LEFTBRACKET", static_cast<int64_t>(KeySym::LEFTBRACKET));
	LuaUtils::pushField(L, "BACKSLASH", static_cast<int64_t>(KeySym::BACKSLASH));
	LuaUtils::pushField(L, "RIGHTBRACKET", static_cast<int64_t>(KeySym::RIGHTBRACKET));
	LuaUtils::pushField(L, "BACKQUOTE", static_cast<int64_t>(KeySym::BACKQUOTE));

	LuaUtils::pushField(L, "A", static_cast<int64_t>(KeySym::A));
	LuaUtils::pushField(L, "B", static_cast<int64_t>(KeySym::B));
	LuaUtils::pushField(L, "C", static_cast<int64_t>(KeySym::C));
	LuaUtils::pushField(L, "D", static_cast<int64_t>(KeySym::D));
	LuaUtils::pushField(L, "E", static_cast<int64_t>(KeySym::E));
	LuaUtils::pushField(L, "F", static_cast<int64_t>(KeySym::F));
	LuaUtils::pushField(L, "G", static_cast<int64_t>(KeySym::G));
	LuaUtils::pushField(L, "H", static_cast<int64_t>(KeySym::H));
	LuaUtils::pushField(L, "I", static_cast<int64_t>(KeySym::I));
	LuaUtils::pushField(L, "J", static_cast<int64_t>(KeySym::J));
	LuaUtils::pushField(L, "K", static_cast<int64_t>(KeySym::K));
	LuaUtils::pushField(L, "L", static_cast<int64_t>(KeySym::L));
	LuaUtils::pushField(L, "M", static_cast<int64_t>(KeySym::M));
	LuaUtils::pushField(L, "N", static_cast<int64_t>(KeySym::N));
	LuaUtils::pushField(L, "O", static_cast<int64_t>(KeySym::O));
	LuaUtils::pushField(L, "P", static_cast<int64_t>(KeySym::P));
	LuaUtils::pushField(L, "Q", static_cast<int64_t>(KeySym::Q));
	LuaUtils::pushField(L, "R", static_cast<int64_t>(KeySym::R));
	LuaUtils::pushField(L, "S", static_cast<int64_t>(KeySym::S));
	LuaUtils::pushField(L, "T", static_cast<int64_t>(KeySym::T));
	LuaUtils::pushField(L, "U", static_cast<int64_t>(KeySym::U));
	LuaUtils::pushField(L, "V", static_cast<int64_t>(KeySym::V));
	LuaUtils::pushField(L, "W", static_cast<int64_t>(KeySym::W));
	LuaUtils::pushField(L, "X", static_cast<int64_t>(KeySym::X));
	LuaUtils::pushField(L, "Y", static_cast<int64_t>(KeySym::Y));
	LuaUtils::pushField(L, "Z", static_cast<int64_t>(KeySym::Z));
	LuaUtils::pushField(L, "DELETE", static_cast<int64_t>(KeySym::DELETE));

	LuaUtils::pushField(L, "KP0", static_cast<int64_t>(KeySym::KP0));
	LuaUtils::pushField(L, "KP1", static_cast<int64_t>(KeySym::KP1));
	LuaUtils::pushField(L, "KP2", static_cast<int64_t>(KeySym::KP2));
	LuaUtils::pushField(L, "KP3", static_cast<int64_t>(KeySym::KP3));
	LuaUtils::pushField(L, "KP4", static_cast<int64_t>(KeySym::KP4));
	LuaUtils::pushField(L, "KP5", static_cast<int64_t>(KeySym::KP5));
	LuaUtils::pushField(L, "KP6", static_cast<int64_t>(KeySym::KP6));
	LuaUtils::pushField(L, "KP7", static_cast<int64_t>(KeySym::KP7));
	LuaUtils::pushField(L, "KP8", static_cast<int64_t>(KeySym::KP8));
	LuaUtils::pushField(L, "KP9", static_cast<int64_t>(KeySym::KP9));
	LuaUtils::pushField(L, "KP_PERIOD", static_cast<int64_t>(KeySym::KP_PERIOD));
	LuaUtils::pushField(L, "KP_DIVIDE", static_cast<int64_t>(KeySym::KP_DIVIDE));
	LuaUtils::pushField(L, "KP_MULTIPLY", static_cast<int64_t>(KeySym::KP_MULTIPLY));
	LuaUtils::pushField(L, "KP_MINUS", static_cast<int64_t>(KeySym::KP_MINUS));
	LuaUtils::pushField(L, "KP_PLUS", static_cast<int64_t>(KeySym::KP_PLUS));
	LuaUtils::pushField(L, "KP_ENTER", static_cast<int64_t>(KeySym::KP_ENTER));
	LuaUtils::pushField(L, "KP_EQUALS", static_cast<int64_t>(KeySym::KP_EQUALS));

	LuaUtils::pushField(L, "UP", static_cast<int64_t>(KeySym::UP));
	LuaUtils::pushField(L, "DOWN", static_cast<int64_t>(KeySym::DOWN));
	LuaUtils::pushField(L, "RIGHT", static_cast<int64_t>(KeySym::RIGHT));
	LuaUtils::pushField(L, "LEFT", static_cast<int64_t>(KeySym::LEFT));
	LuaUtils::pushField(L, "INSERT", static_cast<int64_t>(KeySym::INSERT));
	LuaUtils::pushField(L, "HOME", static_cast<int64_t>(KeySym::HOME));
	LuaUtils::pushField(L, "END", static_cast<int64_t>(KeySym::END));
	LuaUtils::pushField(L, "PAGEUP", static_cast<int64_t>(KeySym::PAGEUP));
	LuaUtils::pushField(L, "PAGEDOWN", static_cast<int64_t>(KeySym::PAGEDOWN));

	LuaUtils::pushField(L, "F1", static_cast<int64_t>(KeySym::F1));
	LuaUtils::pushField(L, "F2", static_cast<int64_t>(KeySym::F2));
	LuaUtils::pushField(L, "F3", static_cast<int64_t>(KeySym::F3));
	LuaUtils::pushField(L, "F4", static_cast<int64_t>(KeySym::F4));
	LuaUtils::pushField(L, "F5", static_cast<int64_t>(KeySym::F5));
	LuaUtils::pushField(L, "F6", static_cast<int64_t>(KeySym::F6));
	LuaUtils::pushField(L, "F7", static_cast<int64_t>(KeySym::F7));
	LuaUtils::pushField(L, "F8", static_cast<int64_t>(KeySym::F8));
	LuaUtils::pushField(L, "F9", static_cast<int64_t>(KeySym::F9));
	LuaUtils::pushField(L, "F10", static_cast<int64_t>(KeySym::F10));
	LuaUtils::pushField(L, "F11", static_cast<int64_t>(KeySym::F11));
	LuaUtils::pushField(L, "F12", static_cast<int64_t>(KeySym::F12));
	LuaUtils::pushField(L, "F13", static_cast<int64_t>(KeySym::F13));
	LuaUtils::pushField(L, "F14", static_cast<int64_t>(KeySym::F14));
	LuaUtils::pushField(L, "F5", static_cast<int64_t>(KeySym::F15));

	LuaUtils::pushField(L, "NUM_LOCK", static_cast<int64_t>(KeySym::NUM_LOCK));
	LuaUtils::pushField(L, "CAPS_LOCK", static_cast<int64_t>(KeySym::CAPS_LOCK));
	LuaUtils::pushField(L, "SCROLL_LOCK", static_cast<int64_t>(KeySym::SCROLL_LOCK));
	LuaUtils::pushField(L, "RSHIFT", static_cast<int64_t>(KeySym::RSHIFT));
	LuaUtils::pushField(L, "LSHIFT", static_cast<int64_t>(KeySym::LSHIFT));
	LuaUtils::pushField(L, "RCTRL", static_cast<int64_t>(KeySym::RCTRL));
	LuaUtils::pushField(L, "LCTRL", static_cast<int64_t>(KeySym::LCTRL));
	LuaUtils::pushField(L, "RALT", static_cast<int64_t>(KeySym::RALT));
	LuaUtils::pushField(L, "LALT", static_cast<int64_t>(KeySym::LALT));
	LuaUtils::pushField(L, "RSUPER", static_cast<int64_t>(KeySym::RSUPER));
	LuaUtils::pushField(L, "LSUPER", static_cast<int64_t>(KeySym::LSUPER));
	LuaUtils::pushField(L, "PRINTSCREEN", static_cast<int64_t>(KeySym::PRINTSCREEN));
	LuaUtils::pushField(L, "PAUSE", static_cast<int64_t>(KeySym::PAUSE));
	LuaUtils::pushField(L, "MENU", static_cast<int64_t>(KeySym::MENU));

	// SDL only keysyms
	LuaUtils::pushField(L, "CLEAR", static_cast<int64_t>(KeySym::CLEAR)); // Android too
	LuaUtils::pushField(L, "EXCLAIM", static_cast<int64_t>(KeySym::EXCLAIM));
	LuaUtils::pushField(L, "QUOTEDBL", static_cast<int64_t>(KeySym::QUOTEDBL));
	LuaUtils::pushField(L, "HASH", static_cast<int64_t>(KeySym::HASH));
	LuaUtils::pushField(L, "DOLLAR", static_cast<int64_t>(KeySym::DOLLAR));
	LuaUtils::pushField(L, "AMPERSAND", static_cast<int64_t>(KeySym::AMPERSAND));
	LuaUtils::pushField(L, "LEFTPAREN", static_cast<int64_t>(KeySym::LEFTPAREN));
	LuaUtils::pushField(L, "RIGHTPAREN", static_cast<int64_t>(KeySym::RIGHTPAREN));
	LuaUtils::pushField(L, "ASTERISK", static_cast<int64_t>(KeySym::ASTERISK));
	LuaUtils::pushField(L, "COLON", static_cast<int64_t>(KeySym::COLON));
	LuaUtils::pushField(L, "LESS", static_cast<int64_t>(KeySym::LESS));
	LuaUtils::pushField(L, "EQUALS", static_cast<int64_t>(KeySym::EQUALS)); // Android too
	LuaUtils::pushField(L, "GREATER", static_cast<int64_t>(KeySym::GREATER));
	LuaUtils::pushField(L, "QUESTION", static_cast<int64_t>(KeySym::QUESTION));
	LuaUtils::pushField(L, "AT", static_cast<int64_t>(KeySym::AT)); // Android too
	LuaUtils::pushField(L, "CARET", static_cast<int64_t>(KeySym::CARET));
	LuaUtils::pushField(L, "UNDERSCORE", static_cast<int64_t>(KeySym::UNDERSCORE));
	LuaUtils::pushField(L, "MODE", static_cast<int64_t>(KeySym::MODE));
	LuaUtils::pushField(L, "APPLICATION", static_cast<int64_t>(KeySym::APPLICATION));
	LuaUtils::pushField(L, "HELP", static_cast<int64_t>(KeySym::HELP));
	LuaUtils::pushField(L, "SYSREQ", static_cast<int64_t>(KeySym::SYSREQ)); // Android too
	LuaUtils::pushField(L, "POWER", static_cast<int64_t>(KeySym::POWER)); // Android too
	LuaUtils::pushField(L, "UNDO", static_cast<int64_t>(KeySym::UNDO));

	// GLFW only keysyms
	LuaUtils::pushField(L, "WORLD1", static_cast<int64_t>(KeySym::WORLD1));
	LuaUtils::pushField(L, "WORLD2", static_cast<int64_t>(KeySym::WORLD2));

	// Android only keysyms
	LuaUtils::pushField(L, "SOFT_LEFT", static_cast<int64_t>(KeySym::SOFT_LEFT));
	LuaUtils::pushField(L, "SOFT_RIGHT", static_cast<int64_t>(KeySym::SOFT_RIGHT));
	LuaUtils::pushField(L, "BACK", static_cast<int64_t>(KeySym::BACK));
	LuaUtils::pushField(L, "CALL", static_cast<int64_t>(KeySym::CALL));
	LuaUtils::pushField(L, "ENDCALL", static_cast<int64_t>(KeySym::ENDCALL));
	LuaUtils::pushField(L, "STAR", static_cast<int64_t>(KeySym::STAR));
	LuaUtils::pushField(L, "POUND", static_cast<int64_t>(KeySym::POUND));
	LuaUtils::pushField(L, "DPAD_CENTER", static_cast<int64_t>(KeySym::DPAD_CENTER));
	LuaUtils::pushField(L, "VOLUME_UP", static_cast<int64_t>(KeySym::VOLUME_UP));
	LuaUtils::pushField(L, "VOLUME_DOWN", static_cast<int64_t>(KeySym::VOLUME_DOWN));
	LuaUtils::pushField(L, "CAMERA", static_cast<int64_t>(KeySym::CAMERA));
	LuaUtils::pushField(L, "SYM", static_cast<int64_t>(KeySym::SYM));
	LuaUtils::pushField(L, "EXPLORER", static_cast<int64_t>(KeySym::EXPLORER));
	LuaUtils::pushField(L, "ENVELOPE", static_cast<int64_t>(KeySym::ENVELOPE));
	LuaUtils::pushField(L, "NUM", static_cast<int64_t>(KeySym::NUM));
	LuaUtils::pushField(L, "HEADSETHOOK", static_cast<int64_t>(KeySym::HEADSETHOOK));
	LuaUtils::pushField(L, "FOCUS", static_cast<int64_t>(KeySym::FOCUS));
	LuaUtils::pushField(L, "NOTIFICATION", static_cast<int64_t>(KeySym::NOTIFICATION));
	LuaUtils::pushField(L, "SEARCH", static_cast<int64_t>(KeySym::SEARCH));
	LuaUtils::pushField(L, "MEDIA_PLAY_PAUSE", static_cast<int64_t>(KeySym::MEDIA_PLAY_PAUSE));
	LuaUtils::pushField(L, "MEDIA_STOP", static_cast<int64_t>(KeySym::MEDIA_STOP));
	LuaUtils::pushField(L, "MEDIA_NEXT", static_cast<int64_t>(KeySym::MEDIA_NEXT));
	LuaUtils::pushField(L, "MEDIA_PREVIOUS", static_cast<int64_t>(KeySym::MEDIA_PREVIOUS));
	LuaUtils::pushField(L, "MEDIA_REWIND", static_cast<int64_t>(KeySym::MEDIA_REWIND));
	LuaUtils::pushField(L, "MEDIA_FAST_FORWARD", static_cast<int64_t>(KeySym::MEDIA_FAST_FORWARD));
	LuaUtils::pushField(L, "MUTE", static_cast<int64_t>(KeySym::MUTE));
	LuaUtils::pushField(L, "PICTSYMBOLS", static_cast<int64_t>(KeySym::PICTSYMBOLS));
	LuaUtils::pushField(L, "SWITCH_CHARSET", static_cast<int64_t>(KeySym::SWITCH_CHARSET));
	LuaUtils::pushField(L, "BUTTON_A", static_cast<int64_t>(KeySym::BUTTON_A));
	LuaUtils::pushField(L, "BUTTON_B", static_cast<int64_t>(KeySym::BUTTON_B));
	LuaUtils::pushField(L, "BUTTON_C", static_cast<int64_t>(KeySym::BUTTON_C));
	LuaUtils::pushField(L, "BUTTON_X", static_cast<int64_t>(KeySym::BUTTON_X));
	LuaUtils::pushField(L, "BUTTON_Y", static_cast<int64_t>(KeySym::BUTTON_Y));
	LuaUtils::pushField(L, "BUTTON_Z", static_cast<int64_t>(KeySym::BUTTON_Z));
	LuaUtils::pushField(L, "BUTTON_L1", static_cast<int64_t>(KeySym::BUTTON_L1));
	LuaUtils::pushField(L, "BUTTON_R1", static_cast<int64_t>(KeySym::BUTTON_R1));
	LuaUtils::pushField(L, "BUTTON_L2", static_cast<int64_t>(KeySym::BUTTON_L2));
	LuaUtils::pushField(L, "BUTTON_R2", static_cast<int64_t>(KeySym::BUTTON_R2));
	LuaUtils::pushField(L, "BUTTON_THUMBL", static_cast<int64_t>(KeySym::BUTTON_THUMBL));
	LuaUtils::pushField(L, "BUTTON_THUMBR", static_cast<int64_t>(KeySym::BUTTON_THUMBR));
	LuaUtils::pushField(L, "BUTTON_START", static_cast<int64_t>(KeySym::BUTTON_START));
	LuaUtils::pushField(L, "BUTTON_SELECT", static_cast<int64_t>(KeySym::BUTTON_SELECT));
	LuaUtils::pushField(L, "BUTTON_MODE", static_cast<int64_t>(KeySym::BUTTON_MODE));
	// From API level 13
	LuaUtils::pushField(L, "FORWARD_DEL", static_cast<int64_t>(KeySym::FORWARD_DEL));
	LuaUtils::pushField(L, "FUNCTION_KEY", static_cast<int64_t>(KeySym::FUNCTION_KEY));
	LuaUtils::pushField(L, "MOVE_HOME", static_cast<int64_t>(KeySym::MOVE_HOME));
	LuaUtils::pushField(L, "MOVE_END", static_cast<int64_t>(KeySym::MOVE_END));
	LuaUtils::pushField(L, "FORWARD", static_cast<int64_t>(KeySym::FORWARD));
	LuaUtils::pushField(L, "MEDIA_PLAY", static_cast<int64_t>(KeySym::MEDIA_PLAY));
	LuaUtils::pushField(L, "MEDIA_PAUSE", static_cast<int64_t>(KeySym::MEDIA_PAUSE));
	LuaUtils::pushField(L, "MEDIA_CLOSE", static_cast<int64_t>(KeySym::MEDIA_CLOSE));
	LuaUtils::pushField(L, "MEDIA_EJECT", static_cast<int64_t>(KeySym::MEDIA_EJECT));
	LuaUtils::pushField(L, "MEDIA_RECORD", static_cast<int64_t>(KeySym::MEDIA_RECORD));
	LuaUtils::pushField(L, "KP_COMMA", static_cast<int64_t>(KeySym::KP_COMMA));
	LuaUtils::pushField(L, "KP_LEFTPAREN", static_cast<int64_t>(KeySym::KP_LEFTPAREN));
	LuaUtils::pushField(L, "KP_RIGHTPAREN", static_cast<int64_t>(KeySym::KP_RIGHTPAREN));
	LuaUtils::pushField(L, "VOLUME_MUTE", static_cast<int64_t>(KeySym::VOLUME_MUTE));
	LuaUtils::pushField(L, "INFO", static_cast<int64_t>(KeySym::INFO));
	LuaUtils::pushField(L, "CHANNEL_UP", static_cast<int64_t>(KeySym::CHANNEL_UP));
	LuaUtils::pushField(L, "CHANNEL_DOWN", static_cast<int64_t>(KeySym::CHANNEL_DOWN));
	LuaUtils::pushField(L, "ZOOM_IN", static_cast<int64_t>(KeySym::ZOOM_IN));
	LuaUtils::pushField(L, "ZOOM_OUT", static_cast<int64_t>(KeySym::ZOOM_OUT));
	LuaUtils::pushField(L, "TV", static_cast<int64_t>(KeySym::TV));
	LuaUtils::pushField(L, "WINDOW", static_cast<int64_t>(KeySym::WINDOW));
	LuaUtils::pushField(L, "GUIDE", static_cast<int64_t>(KeySym::GUIDE));
	LuaUtils::pushField(L, "DVR", static_cast<int64_t>(KeySym::DVR));
	LuaUtils::pushField(L, "BOOKMARK", static_cast<int64_t>(KeySym::BOOKMARK));
	LuaUtils::pushField(L, "CAPTIONS", static_cast<int64_t>(KeySym::CAPTIONS));
	LuaUtils::pushField(L, "SETTINGS", static_cast<int64_t>(KeySym::SETTINGS));
	LuaUtils::pushField(L, "TV_POWER", static_cast<int64_t>(KeySym::TV_POWER));
	LuaUtils::pushField(L, "TV_INPUT", static_cast<int64_t>(KeySym::TV_INPUT));
	LuaUtils::pushField(L, "STB_POWER", static_cast<int64_t>(KeySym::STB_POWER));
	LuaUtils::pushField(L, "STB_INPUT", static_cast<int64_t>(KeySym::STB_INPUT));
	LuaUtils::pushField(L, "AVR_POWER", static_cast<int64_t>(KeySym::AVR_POWER));
	LuaUtils::pushField(L, "AVR_INPUT", static_cast<int64_t>(KeySym::AVR_INPUT));
	LuaUtils::pushField(L, "PROG_RED", static_cast<int64_t>(KeySym::PROG_RED));
	LuaUtils::pushField(L, "PROG_GREEN", static_cast<int64_t>(KeySym::PROG_GREEN));
	LuaUtils::pushField(L, "PROG_YELLOW", static_cast<int64_t>(KeySym::PROG_YELLOW));
	LuaUtils::pushField(L, "PROG_BLUE", static_cast<int64_t>(KeySym::PROG_BLUE));
	LuaUtils::pushField(L, "APP_SWITCH", static_cast<int64_t>(KeySym::APP_SWITCH));
	LuaUtils::pushField(L, "BUTTON_1", static_cast<int64_t>(KeySym::BUTTON_1));
	LuaUtils::pushField(L, "BUTTON_2", static_cast<int64_t>(KeySym::BUTTON_2));
	LuaUtils::pushField(L, "BUTTON_3", static_cast<int64_t>(KeySym::BUTTON_3));
	LuaUtils::pushField(L, "BUTTON_4", static_cast<int64_t>(KeySym::BUTTON_4));
	LuaUtils::pushField(L, "BUTTON_5", static_cast<int64_t>(KeySym::BUTTON_5));
	LuaUtils::pushField(L, "BUTTON_6", static_cast<int64_t>(KeySym::BUTTON_6));
	LuaUtils::pushField(L, "BUTTON_7", static_cast<int64_t>(KeySym::BUTTON_7));
	LuaUtils::pushField(L, "BUTTON_8", static_cast<int64_t>(KeySym::BUTTON_8));
	LuaUtils::pushField(L, "BUTTON_9", static_cast<int64_t>(KeySym::BUTTON_9));
	LuaUtils::pushField(L, "BUTTON_10", static_cast<int64_t>(KeySym::BUTTON_10));
	LuaUtils::pushField(L, "BUTTON_11", static_cast<int64_t>(KeySym::BUTTON_11));
	LuaUtils::pushField(L, "BUTTON_12", static_cast<int64_t>(KeySym::BUTTON_12));
	LuaUtils::pushField(L, "BUTTON_13", static_cast<int64_t>(KeySym::BUTTON_13));
	LuaUtils::pushField(L, "BUTTON_14", static_cast<int64_t>(KeySym::BUTTON_14));
	LuaUtils::pushField(L, "BUTTON_15", static_cast<int64_t>(KeySym::BUTTON_15));
	LuaUtils::pushField(L, "BUTTON_16", static_cast<int64_t>(KeySym::BUTTON_16));

	lua_setfield(L, -2, "keysym");

	lua_createtable(L, static_cast<int>(KeyMod::COUNT), 0);

	LuaUtils::pushField(L, "NONE", static_cast<int64_t>(KeyMod::NONE));

	// Common keymods
	LuaUtils::pushField(L, "SHIFT", static_cast<int64_t>(KeyMod::SHIFT));
	LuaUtils::pushField(L, "CTRL", static_cast<int64_t>(KeyMod::CTRL));
	LuaUtils::pushField(L, "ALT", static_cast<int64_t>(KeyMod::ALT));
	LuaUtils::pushField(L, "SUPER", static_cast<int64_t>(KeyMod::SUPER));

	// Android and SDL only keymods
	LuaUtils::pushField(L, "LSHIFT", static_cast<int64_t>(KeyMod::LSHIFT));
	LuaUtils::pushField(L, "RSHIFT", static_cast<int64_t>(KeyMod::RSHIFT));
	LuaUtils::pushField(L, "LCTRL", static_cast<int64_t>(KeyMod::LCTRL));
	LuaUtils::pushField(L, "RCTRL", static_cast<int64_t>(KeyMod::RCTRL));
	LuaUtils::pushField(L, "LALT", static_cast<int64_t>(KeyMod::LALT));
	LuaUtils::pushField(L, "RALT", static_cast<int64_t>(KeyMod::RALT));
	LuaUtils::pushField(L, "LSUPER", static_cast<int64_t>(KeyMod::LSUPER));
	LuaUtils::pushField(L, "RSUPER", static_cast<int64_t>(KeyMod::RSUPER));
	LuaUtils::pushField(L, "NUM", static_cast<int64_t>(KeyMod::NUM));
	LuaUtils::pushField(L, "CAPS", static_cast<int64_t>(KeyMod::CAPS));

	// SDL only keymods
	LuaUtils::pushField(L, "MODE", static_cast<int64_t>(KeyMod::MODE));

	// Android only keymods
	LuaUtils::pushField(L, "SYM", static_cast<int64_t>(KeyMod::SYM));

	lua_setfield(L, -2, "keymod");

}

}
