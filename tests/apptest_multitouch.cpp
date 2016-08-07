#include "apptest_multitouch.h"
#include "Application.h"
#include "ServiceLocator.h"
#include "ncString.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include "TextNode.h"
#include "IFile.h" // for dataPath()

namespace {

#ifdef __ANDROID__
	const float VerticalTextPos = 0.5f;
	const char *TextureFile = "texture4.webp";
	const char *FontTextureFile = "DroidSans32_256_8888.ktx";
#else
	const float VerticalTextPos = 0.3f;
	const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

}

nc::IAppEventHandler *createAppEventHandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	multitouchString_ = new nc::String(MaxNumChars);

#ifdef __ANDROID__
	texture_ = new nc::Texture((nc::IFile::dataPath() + "textures/" + TextureFile).data());
	for (unsigned int i = 0; i < nc::TouchEvent::MaxPointers; i++)
	{
		sprites_[i] = new nc::Sprite(&rootNode, texture_);
		sprites_[i]->setScale(0.5f);
		sprites_[i]->enableDraw(false);
	}
#endif

	font_ = new nc::Font((nc::IFile::dataPath() + "fonts/" + FontTextureFile).data(),
	                     (nc::IFile::dataPath() + "fonts/" + FontFntFile).data());
	textNode_ = new nc::TextNode(&rootNode, font_, MaxNumChars);
	textNode_->setScale(0.85f);
	textNode_->setPosition(nc::theApplication().width() * 0.1f, nc::theApplication().height() * VerticalTextPos);

#ifdef __ANDROID__
	multitouchString_->append("Waiting for the first touch event");
#else
	multitouchString_->append("Touch events are only supported on Android");
#endif
}

void MyEventHandler::onFrameStart()
{
	textNode_->setString(*multitouchString_);
}

void MyEventHandler::onShutdown()
{
	delete textNode_;
	delete font_;

#ifdef __ANDROID__
	for (unsigned int i = 0; i < nc::TouchEvent::MaxPointers; i++)
	{
		delete sprites_[i];
	}
	delete texture_;
#endif

	delete multitouchString_;
}

#ifdef __ANDROID__
void MyEventHandler::handleEvent(const nc::TouchEvent &event, nc::String *string, const char *eventName)
{
	string->clear();
	string->formatAppend("count: %d, actionIndex:%d, event: %s\n", event.count, event.actionIndex, eventName);
	for (unsigned int i = 0; i < event.count && i < nc::TouchEvent::MaxPointers; i++)
	{
		const nc::TouchEvent::Pointer &pointer = event.pointers[i];
		string->formatAppend("Touch %d - id:%d, x:%.2f, y:%.2f\n", i, pointer.id, pointer.x, pointer.y);

		sprites_[i]->enableDraw(true);
		sprites_[i]->setPosition(pointer.x, pointer.y);
	}
	for (unsigned int i = event.count; i < nc::TouchEvent::MaxPointers; i++)
	{
		sprites_[i]->enableDraw(false);
	}
}

void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	handleEvent(event, multitouchString_, "onTouchDown");
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	handleEvent(event, multitouchString_, "onTouchUp");
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	handleEvent(event, multitouchString_, "onTouchMove");
}

void MyEventHandler::onPointerDown(const nc::TouchEvent &event)
{
	handleEvent(event, multitouchString_, "onPointerDown");
}

void MyEventHandler::onPointerUp(const nc::TouchEvent &event)
{
	handleEvent(event, multitouchString_, "onPointerUp");
}
#else
void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KEY_ESCAPE || event.sym == nc::KEY_Q)
	{
		nc::theApplication().quit();
	}
	else if (event.sym == nc::KEY_SPACE)
	{
		nc::theApplication().togglePause();
	}
}
#endif
