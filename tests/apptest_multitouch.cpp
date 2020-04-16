#include "apptest_multitouch.h"
#include <ncine/Application.h>
#include <nctl/String.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const float VerticalTextPos = 0.5f;
const char *TextureFile = "texture4_ETC2.ktx";
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const float VerticalTextPos = 0.3f;
const char *TextureFile = "texture4.png";
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";
const float SpriteScale = 0.5f;

const unsigned int LongNumChars = 1024;
const unsigned int ShortNumChars = 128;

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	multitouchString_ = nctl::makeUnique<nctl::String>(LongNumChars);

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());
	textNode_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get(), LongNumChars);
	textNode_->setScale(0.85f);

#if HAS_TOUCH
	texture_ = nctl::makeUnique<nc::Texture>(prefixDataPath("textures", TextureFile).data());
	for (unsigned int i = 0; i < nc::TouchEvent::MaxPointers; i++)
	{
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(&rootNode, texture_.get()));
		sprites_.back()->setScale(SpriteScale);
		sprites_.back()->setEnabled(false);
		spriteTexts_.pushBack(nctl::makeUnique<nc::TextNode>(sprites_[i].get(), font_.get(), ShortNumChars));
		spriteTexts_.back()->setPosition(0.0f, sprites_[i]->height() * 1.25f);
	}
#endif

#if HAS_TOUCH
	multitouchString_->append("Waiting for the first touch event");
#else
	multitouchString_->append("Touch events are not supported on this backend");
#endif
}

void MyEventHandler::onFrameStart()
{
	textNode_->setString(*multitouchString_);
	textNode_->setPosition(nc::theApplication().width() * 0.1f + textNode_->width() * 0.5f,
	                       nc::theApplication().height() * VerticalTextPos - textNode_->height() * 0.5f);
}

#if HAS_TOUCH
void MyEventHandler::handleEvent(const nc::TouchEvent &event, nctl::String *string, const char *eventName)
{
	static nctl::String touchString(ShortNumChars);

	string->clear();
	string->formatAppend("count: %d, actionIndex:%d, event: %s\n", event.count, event.actionIndex, eventName);
	for (unsigned int i = 0; i < event.count && i < nc::TouchEvent::MaxPointers; i++)
	{
		const nc::TouchEvent::Pointer &pointer = event.pointers[i];
		string->formatAppend("Touch %d - id:%d, x:%.2f, y:%.2f, pressure:%.2f\n", i, pointer.id, pointer.x, pointer.y, pointer.pressure);

		sprites_[i]->setEnabled(true);
		sprites_[i]->setPosition(pointer.x, pointer.y);
		sprites_[i]->setScale(SpriteScale * pointer.pressure);

		touchString.format("%d - id:%d, <%.2f x %.2f> p:%.2f\n", i, pointer.id, pointer.x, pointer.y, pointer.pressure);
		spriteTexts_[i]->setString(touchString);
	}
	for (unsigned int i = event.count; i < nc::TouchEvent::MaxPointers; i++)
		sprites_[i]->setEnabled(false);
}

void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	handleEvent(event, multitouchString_.get(), "onTouchDown");
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	handleEvent(event, multitouchString_.get(), "onTouchUp");
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	handleEvent(event, multitouchString_.get(), "onTouchMove");
}

void MyEventHandler::onPointerDown(const nc::TouchEvent &event)
{
	handleEvent(event, multitouchString_.get(), "onPointerDown");
}

void MyEventHandler::onPointerUp(const nc::TouchEvent &event)
{
	handleEvent(event, multitouchString_.get(), "onPointerUp");
}
#endif

#ifndef __ANDROID__
void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
	{
		const bool isSuspended = nc::theApplication().isSuspended();
		nc::theApplication().setSuspended(!isSuspended);
	}
}
#endif
