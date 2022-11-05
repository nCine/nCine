#include "LuaTextNode.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaDrawableNode.h"
#include "LuaUtils.h"
#include "LuaVector2Utils.h"
#include "TextNode.h"

namespace ncine {

namespace LuaNames {
namespace TextNode {
	static const char *TextNode = "textnode";

	static const char *width = "get_width";
	static const char *height = "get_height";
	static const char *absWidth = "get_abswidth";
	static const char *absHeight = "get_absheight";

	static const char *font = "get_font";
	static const char *setFont = "set_font";

	static const char *renderMode = "get_render_mode";
	static const char *setRenderMode = "set_render_mode";

	static const char *withKerning = "get_kerning";
	static const char *enableKerning = "set_kerning";

	static const char *alignment = "get_alignment";
	static const char *setAlignment = "set_alignment";

	static const char *lineHeight = "get_lineheight";
	static const char *absLineHeight = "get_abslineheight";
	static const char *setLineHeight = "set_lineheight";

	static const char *string = "get_string";
	static const char *setString = "set_string";

	static const char *calculateBoundaries = "calculate_boundaries";

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
		LuaUtils::addFunction(L, LuaNames::cloneNode, cloneNode);
	}

	LuaUtils::addFunction(L, LuaNames::TextNode::width, width);
	LuaUtils::addFunction(L, LuaNames::TextNode::height, height);
	LuaUtils::addFunction(L, LuaNames::TextNode::absWidth, absWidth);
	LuaUtils::addFunction(L, LuaNames::TextNode::absHeight, absHeight);

	LuaUtils::addFunction(L, LuaNames::TextNode::font, font);
	LuaUtils::addFunction(L, LuaNames::TextNode::setFont, setFont);

	LuaUtils::addFunction(L, LuaNames::TextNode::renderMode, renderMode);
	LuaUtils::addFunction(L, LuaNames::TextNode::setRenderMode, setRenderMode);

	LuaUtils::addFunction(L, LuaNames::TextNode::withKerning, withKerning);
	LuaUtils::addFunction(L, LuaNames::TextNode::enableKerning, enableKerning);

	LuaUtils::addFunction(L, LuaNames::TextNode::alignment, alignment);
	LuaUtils::addFunction(L, LuaNames::TextNode::setAlignment, setAlignment);

	LuaUtils::addFunction(L, LuaNames::TextNode::lineHeight, lineHeight);
	LuaUtils::addFunction(L, LuaNames::TextNode::absLineHeight, absLineHeight);
	LuaUtils::addFunction(L, LuaNames::TextNode::setLineHeight, setLineHeight);

	LuaUtils::addFunction(L, LuaNames::TextNode::string, string);
	LuaUtils::addFunction(L, LuaNames::TextNode::setString, setString);

	LuaUtils::addFunction(L, LuaNames::TextNode::calculateBoundaries, calculateBoundaries);

	LuaDrawableNode::exposeFunctions(L);

	lua_setfield(L, -2, LuaNames::TextNode::TextNode);
}

void LuaTextNode::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 3);

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
	SceneNode *parent = LuaUntrackedUserData<SceneNode>::retrieveOrNil(L, -3);
	Font *font = LuaUntrackedUserData<Font>::retrieveOrNil(L, -2);
	const unsigned int maxStringLength = LuaUtils::retrieve<uint32_t>(L, -1);

	LuaClassTracker<TextNode>::newObject(L, parent, font, maxStringLength);

	return 1;
}

int LuaTextNode::cloneNode(lua_State *L)
{
	const TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaClassTracker<TextNode>::cloneNode(L, *textnode);
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::width(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUtils::push(L, textnode->width());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::height(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUtils::push(L, textnode->height());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::absWidth(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUtils::push(L, textnode->absWidth());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::absHeight(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUtils::push(L, textnode->absHeight());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::font(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUntrackedUserData<Font>::push(L, textnode->font());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::setFont(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -2);
	Font *font = LuaUntrackedUserData<Font>::retrieveOrNil(L, -1);

	if (textnode)
		textnode->setFont(font);
	else
		LuaUtils::pushNil(L);

	return 0;
}

int LuaTextNode::renderMode(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUtils::push(L, static_cast<int64_t>(textnode->renderMode()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::setRenderMode(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -2);
	const Font::RenderMode renderMode = static_cast<Font::RenderMode>(LuaUtils::retrieve<int64_t>(L, -1));

	if (textnode)
		textnode->setRenderMode(renderMode);

	return 0;
}

int LuaTextNode::withKerning(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUtils::push(L, textnode->withKerning());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::enableKerning(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -2);
	const bool withKerning = LuaUtils::retrieve<bool>(L, -1);

	if (textnode)
		textnode->enableKerning(withKerning);

	return 0;
}

int LuaTextNode::alignment(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUtils::push(L, static_cast<int64_t>(textnode->alignment()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::setAlignment(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -2);
	const TextNode::Alignment alignment = static_cast<TextNode::Alignment>(LuaUtils::retrieve<int64_t>(L, -1));

	if (textnode)
		textnode->setAlignment(alignment);

	return 0;
}

int LuaTextNode::lineHeight(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUtils::push(L, textnode->lineHeight());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::absLineHeight(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUtils::push(L, textnode->absLineHeight());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::setLineHeight(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -2);
	const float lineHeight = LuaUtils::retrieve<float>(L, -1);

	if (textnode)
		textnode->setLineHeight(lineHeight);

	return 0;
}

int LuaTextNode::string(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -1);

	if (textnode)
		LuaUtils::push(L, textnode->string().data());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTextNode::setString(lua_State *L)
{
	TextNode *textnode = LuaUntrackedUserData<TextNode>::retrieve(L, -2);
	const char *string = LuaUtils::retrieve<const char *>(L, -1);

	if (textnode)
		textnode->setString(string);

	return 0;
}

int LuaTextNode::calculateBoundaries(lua_State *L)
{
	const Font *font = LuaUntrackedUserData<Font>::retrieve(L, -3);
	const bool withKerning = LuaUtils::retrieve<bool>(L, -2);
	const char *string = LuaUtils::retrieve<const char *>(L, -1);

	if (font)
	{
		const Vector2f boundaries = TextNode::calculateBoundaries(*font, withKerning, string);
		LuaVector2fUtils::push(L, boundaries);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

}
