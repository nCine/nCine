#include "apptest_font.h"
#include "Application.h"
#include "IAppEventHandler.h"
#include "TextNode.h"
#include "IFile.h" // for dataPath()
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
	const char *Font1TextureFile = "DroidSans32_256.webp";
	const char *Font2TextureFile = "NotoSerif-Regular32_256.webp";
	const char *Font3TextureFile = "Roboto-Regular32_256.webp";
#else
	const char *Font1TextureFile = "DroidSans32_256.png";
	const char *Font2TextureFile = "NotoSerif-Regular32_256.png";
	const char *Font3TextureFile = "Roboto-Regular32_256.png";
#endif
const char *Font1FntFile = "DroidSans32_256.fnt";
const char *Font2FntFile = "NotoSerif-Regular32_256.fnt";
const char *Font3FntFile = "Roboto-Regular32_256.fnt";

}

nc::IAppEventHandler *createAppEventHandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	font1_ = new nc::Font((nc::IFile::dataPath() + "fonts/" + Font1TextureFile).data(),
	                      (nc::IFile::dataPath() + "fonts/" + Font1FntFile).data());
	font2_ = new nc::Font((nc::IFile::dataPath() + "fonts/" + Font2TextureFile).data(),
	                      (nc::IFile::dataPath() + "fonts/" + Font2FntFile).data());
	font3_ = new nc::Font((nc::IFile::dataPath() + "fonts/" + Font3TextureFile).data(),
	                      (nc::IFile::dataPath() + "fonts/" + Font3FntFile).data());

	char testString[] = "WAY.P.ATAV";
	float textHeight = nc::theApplication().height() * 0.8f;
	float screenWidth = nc::theApplication().width();
	text1_ = new nc::TextNode(&rootNode, font1_);
	text1_->setScale(2.0f);
	text1_->setString(testString);
	textHeight -= text1_->height() * 0.5f;
	text1_->setPosition(screenWidth * 0.5f, textHeight);
	text1_->setColor(255, 0, 0, 255);

	text2_ = new nc::TextNode(&rootNode, font1_);
	text2_->setScale(2.0f);
	text2_->setString(testString);
	textHeight -= text1_->height();
	text2_->setPosition(screenWidth * 0.5f, textHeight);
	text2_->enableKerning(false);
	text2_->setColor(255, 0, 0, 128);

	text3_ = new nc::TextNode(&rootNode, font2_);
	text3_->setScale(2.0f);
	text3_->setString(testString);
	textHeight -= text2_->height();
	text3_->setPosition(screenWidth * 0.5f, textHeight);
	text3_->setColor(0, 255, 0, 255);

	text4_ = new nc::TextNode(&rootNode, font2_);
	text4_->setScale(2.0f);
	text4_->setString(testString);
	textHeight -= text3_->height();
	text4_->setPosition(screenWidth * 0.5f, textHeight);
	text4_->enableKerning(false);
	text4_->setColor(0, 255, 0, 128);

	text5_ = new nc::TextNode(&rootNode, font3_);
	text5_->setScale(2.0f);
	text5_->setString(testString);
	textHeight -= text4_->height();
	text5_->setPosition(screenWidth * 0.5f, textHeight);
	text5_->setColor(0, 0, 255, 255);

	text6_ = new nc::TextNode(&rootNode, font3_);
	text6_->setScale(2.0f);
	text6_->setString(testString);
	textHeight -= text5_->height();
	text6_->setPosition(screenWidth * 0.5f, textHeight);
	text6_->enableKerning(false);
	text6_->setColor(0, 0, 255, 128);
}

void MyEventHandler::onShutdown()
{
	delete text6_;
	delete text5_;
	delete text4_;
	delete text3_;
	delete text2_;
	delete text1_;

	delete font3_;
	delete font2_;
	delete font1_;
}

#ifndef __ANDROID__
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
