#include "apptest_font.h"
#include <ncine/Application.h>
#include <ncine/IAppEventHandler.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *Font1TextureFile = "DroidSans32_256_ETC2.ktx";
const char *Font2TextureFile = "NotoSerif-Regular32_256_ETC2.ktx";
const char *Font3TextureFile = "Roboto-Regular32_256_ETC2.ktx";
#else
const char *Font1TextureFile = "DroidSans32_256.png";
const char *Font2TextureFile = "NotoSerif-Regular32_256.png";
const char *Font3TextureFile = "Roboto-Regular32_256.png";
#endif
const char *Font1FntFile = "DroidSans32_256.fnt";
const char *Font2FntFile = "NotoSerif-Regular32_256.fnt";
const char *Font3FntFile = "Roboto-Regular32_256.fnt";

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

	fonts_.pushBack(nctl::makeUnique<nc::Font>((prefixDataPath("fonts", Font1FntFile)).data(),
	                                           (prefixDataPath("fonts", Font1TextureFile)).data()));
	fonts_.pushBack(nctl::makeUnique<nc::Font>((prefixDataPath("fonts", Font2FntFile)).data(),
	                                           (prefixDataPath("fonts", Font2TextureFile)).data()));
	fonts_.pushBack(nctl::makeUnique<nc::Font>((prefixDataPath("fonts", Font3FntFile)).data(),
	                                           (prefixDataPath("fonts", Font3TextureFile)).data()));

	const char testString[] = "WAY.P.ATAV";
	float textHeight = nc::theApplication().height() * 0.8f;
	const float screenWidth = nc::theApplication().width();
	texts_.pushBack(nctl::makeUnique<nc::TextNode>(&rootNode, fonts_[0].get()));
	texts_[0]->setScale(2.0f);
	texts_[0]->setString(testString);
	textHeight -= texts_[0]->height() * 0.5f;
	texts_[0]->setPosition(screenWidth * 0.5f, textHeight);
	texts_[0]->setColor(255, 0, 0, 255);

	texts_.pushBack(nctl::makeUnique<nc::TextNode>(&rootNode, fonts_[0].get()));
	texts_[1]->setScale(2.0f);
	texts_[1]->setString(testString);
	textHeight -= texts_[0]->height();
	texts_[1]->setPosition(screenWidth * 0.5f, textHeight);
	texts_[1]->enableKerning(false);
	texts_[1]->setColor(255, 0, 0, 128);

	texts_.pushBack(nctl::makeUnique<nc::TextNode>(&rootNode, fonts_[1].get()));
	texts_[2]->setScale(2.0f);
	texts_[2]->setString(testString);
	textHeight -= texts_[1]->height();
	texts_[2]->setPosition(screenWidth * 0.5f, textHeight);
	texts_[2]->setColor(0, 255, 0, 255);

	texts_.pushBack(nctl::makeUnique<nc::TextNode>(&rootNode, fonts_[1].get()));
	texts_[3]->setScale(2.0f);
	texts_[3]->setString(testString);
	textHeight -= texts_[2]->height();
	texts_[3]->setPosition(screenWidth * 0.5f, textHeight);
	texts_[3]->enableKerning(false);
	texts_[3]->setColor(0, 255, 0, 128);

	texts_.pushBack(nctl::makeUnique<nc::TextNode>(&rootNode, fonts_[2].get()));
	texts_[4]->setScale(2.0f);
	texts_[4]->setString(testString);
	textHeight -= texts_[3]->height();
	texts_[4]->setPosition(screenWidth * 0.5f, textHeight);
	texts_[4]->setColor(0, 0, 255, 255);

	texts_.pushBack(nctl::makeUnique<nc::TextNode>(&rootNode, fonts_[2].get()));
	texts_[5]->setScale(2.0f);
	texts_[5]->setString(testString);
	textHeight -= texts_[4]->height();
	texts_[5]->setPosition(screenWidth * 0.5f, textHeight);
	texts_[5]->enableKerning(false);
	texts_[5]->setColor(0, 0, 255, 128);
}

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
