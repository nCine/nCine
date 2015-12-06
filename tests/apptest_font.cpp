#include "apptest_font.h"
#include "Application.h"
#include "IAppEventHandler.h"
#include "Font.h"
#include "TextNode.h"
#include "IFile.h" // for dataPath()

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

#ifdef __ANDROID__
	font1_ = new nc::Font((nc::IFile::dataPath() + "trebuchet32_256_4444.pvr").data(), (nc::IFile::dataPath() + "trebuchet32_256.fnt").data());
	font2_ = new nc::Font((nc::IFile::dataPath() + "arial32_256_4444.pvr").data(), (nc::IFile::dataPath() + "arial32_256.fnt").data());
	font3_ = new nc::Font((nc::IFile::dataPath() + "verdana32_256_4444.pvr").data(), (nc::IFile::dataPath() + "verdana32_256.fnt").data());
#else
	font1_ = new nc::Font((nc::IFile::dataPath() + "fonts/trebuchet32_256_gray.png").data(), (nc::IFile::dataPath() + "fonts/trebuchet32_256.fnt").data());
	font2_ = new nc::Font((nc::IFile::dataPath() + "fonts/arial32_256_gray.png").data(), (nc::IFile::dataPath() + "fonts/arial32_256.fnt").data());
	font3_ = new nc::Font((nc::IFile::dataPath() + "fonts/verdana32_256_gray.png").data(), (nc::IFile::dataPath() + "fonts/verdana32_256.fnt").data());
#endif

	char testString[] = "WAY.P.ATAV";
	float textHeight = nc::theApplication().height() * 0.7f;
	int screenWidth = nc::theApplication().width();
	text1_ = new nc::TextNode(&rootNode, font1_);
	text1_->setScale(2.0f);
	text1_->setString(testString);
	text1_->setPosition((screenWidth - text1_->width()) * 0.5f, textHeight);
	text1_->setColor(255, 0, 0, 255);

	text2_ = new nc::TextNode(&rootNode, font1_);
	text2_->setScale(2.0f);
	text2_->setString(testString);
	textHeight -= text2_->fontBase();
	text2_->setPosition((screenWidth - text1_->width()) * 0.5f, textHeight);
	text2_->enableKerning(false);
	text2_->setColor(255, 0, 0, 128);

	text3_ = new nc::TextNode(&rootNode, font2_);
	text3_->setScale(2.0f);
	text3_->setString(testString);
	textHeight -= text3_->fontBase();
	text3_->setPosition((screenWidth - text3_->width()) * 0.5f, textHeight);
	text3_->setColor(0, 255, 0, 255);

	text4_ = new nc::TextNode(&rootNode, font2_);
	text4_->setScale(2.0f);
	text4_->setString(testString);
	textHeight -= text4_->fontBase();
	text4_->setPosition((screenWidth - text3_->width()) * 0.5f, textHeight);
	text4_->enableKerning(false);
	text4_->setColor(0, 255, 0, 128);

	text5_ = new nc::TextNode(&rootNode, font3_);
	text5_->setScale(2.0f);
	text5_->setString(testString);
	textHeight -= text5_->fontBase();
	text5_->setPosition((screenWidth - text5_->width()) * 0.5f, textHeight);
	text5_->setColor(0, 0, 255, 255);

	text6_ = new nc::TextNode(&rootNode, font3_);
	text6_->setScale(2.0f);
	text6_->setString(testString);
	textHeight -= text6_->fontBase();
	text6_->setPosition((screenWidth - text5_->width()) * 0.5f, textHeight);
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
