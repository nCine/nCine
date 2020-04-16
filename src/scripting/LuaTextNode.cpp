#include "LuaTextNode.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaDrawableNode.h"
#include "LuaUtils.h"
#include "TextNode.h"

namespace ncine {

namespace LuaNames {
namespace TextNode {
	static const char *TextNode = "textnode";

	static const char *width = "get_width";
	static const char *height = "get_height";
	static const char *absWidth = "get_abswidth";
	static const char *absHeight = "get_absheight";

	static const char *withKerning = "get_kerning";
	static const char *enableKerning = "set_kerning";

	static const char *alignment = "get_alignment";
	static const char *setAlignment = "set_alignment";

	static const char *fontBase = "get_fontbase";
	static const char *fontLineHeight = "get_fontlineheight";

	static const char *setString = "set_string";

	static const char *LEFT = "LEFT";
	static const char *CENTER = "CENTER";
	static const char *RIGHT = "RIGHT";
	static const char *Alignment = "text_alignment";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaTextNode::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_newtable(L);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<TextNode>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::TextNode::width, width);
	LuaUtils::addFunction(L, LuaNames::TextNode::height, height);
	LuaUtils::addFunction(L, LuaNames::TextNode::absWidth, absWidth);
	LuaUtils::addFunction(L, LuaNames::TextNode::absHeight, absHeight);

	LuaUtils::addFunction(L, LuaNames::TextNode::withKerning, withKerning);
	LuaUtils::addFunction(L, LuaNames::TextNode::enableKerning, enableKerning);

	LuaUtils::addFunction(L, LuaNames::TextNode::alignment, alignment);
	LuaUtils::addFunction(L, LuaNames::TextNode::setAlignment, setAlignment);

	LuaUtils::addFunction(L, LuaNames::TextNode::fontBase, fontBase);
	LuaUtils::addFunction(L, LuaNames::TextNode::fontLineHeight, fontLineHeight);

	LuaUtils::addFunction(L, LuaNames::TextNode::setString, setString);

	LuaDrawableNode::exposeFunctions(L);

	lua_setfield(L, -2, LuaNames::TextNode::TextNode);
}

void LuaTextNode::exposeConstants(lua_State *L)
{
	lua_createtable(L, 3, 0);

	LuaUtils::pushField(L, LuaNames::TextNode::LEFT, static_cast<int64_t>(TextNode::Alignment::LEFT));
	LuaUtils::pushField(L, LuaNames::TextNode::CENTER, static_cast<int64_t>(TextNode::Alignment::CENTER));
	LuaUtils::pushField(L, LuaNames::TextNode::RIGHT, static_cast<int64_t>(TextNode::Alignment::RIGHT));

	lua_setfield(L, -2, LuaNames::TextNode::Alignment);
}

void LuaTextNode::release(void *object)
{
	TextNode *textnode = reinterpret_cast<TextNode *>(object);
	delete textnode;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaTextNode::newObject(lua_State *L)
{
	SceneNode *parent = LuaClassWrapper<SceneNode>::unwrapUserDataOrNil(L, -3);
	Font *font = LuaClassWrapper<Font>::unwrapUserData(L, -2);
	const unsigned int maxStringLength = LuaUtils::retrieve<uint32_t>(L, -1);

	LuaClassTracker<TextNode>::newObject(L, parent, font, maxStringLength);

	return 1;
}

int LuaTextNode::width(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -1);

	LuaUtils::push(L, textnode->width());

	return 1;
}

int LuaTextNode::height(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -1);

	LuaUtils::push(L, textnode->height());

	return 1;
}

int LuaTextNode::absWidth(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -1);

	LuaUtils::push(L, textnode->absWidth());

	return 1;
}

int LuaTextNode::absHeight(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -1);

	LuaUtils::push(L, textnode->absHeight());

	return 1;
}

int LuaTextNode::withKerning(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -1);

	LuaUtils::push(L, textnode->withKerning());

	return 1;
}

int LuaTextNode::enableKerning(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -2);
	const bool withKerning = LuaUtils::retrieve<bool>(L, -1);

	textnode->enableKerning(withKerning);

	return 0;
}

int LuaTextNode::alignment(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -1);

	LuaUtils::push(L, static_cast<int64_t>(textnode->alignment()));

	return 1;
}

int LuaTextNode::setAlignment(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -2);
	const TextNode::Alignment alignment = static_cast<TextNode::Alignment>(LuaUtils::retrieve<int64_t>(L, -1));

	textnode->setAlignment(alignment);

	return 0;
}

int LuaTextNode::fontBase(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -1);

	LuaUtils::push(L, textnode->fontBase());

	return 1;
}

int LuaTextNode::fontLineHeight(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -1);

	LuaUtils::push(L, textnode->fontLineHeight());

	return 1;
}

int LuaTextNode::setString(lua_State *L)
{
	TextNode *textnode = LuaClassWrapper<TextNode>::unwrapUserData(L, -2);
	const char *string = LuaUtils::retrieve<const char *>(L, -1);

	textnode->setString(string);

	return 0;
}

}
