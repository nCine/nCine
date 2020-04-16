#include "apptest_sinescroller.h"
#include <ncine/Application.h>
#include <ncine/IAppEventHandler.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const float CharScale = 2.0f;

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

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());

	debugString_ = nctl::makeUnique<nctl::String>(128);
	debugText_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get());
	debugText_->setPosition((nc::theApplication().width() - debugText_->width()) * 0.5f,
	                        nc::theApplication().height() - debugText_->fontLineHeight() * 0.5f * 2.0f);
	debugText_->setColor(255, 255, 0, 255);
	debugText_->setAlignment(nc::TextNode::Alignment::CENTER);

	nctl::String string("apptest_sinescroller");
	const unsigned int strLength = string.length();

	char singleChar[2];
	singleChar[1] = '\0';
	const float numCharsInv = 1.0f / static_cast<float>(NumChars);
	for (unsigned int i = 0; i < NumChars; i++)
	{
		singleChar[0] = string[i % strLength];
		characters_.pushBack(nctl::makeUnique<nc::TextNode>(&rootNode, font_.get(), 2));
		characters_.back()->setScale(CharScale);
		characters_.back()->enableKerning(false);
		characters_.back()->setString(singleChar);
		characters_.back()->setColorF((NumChars - i) * numCharsInv, 0.25f, (i + 1) * numCharsInv, 1.0f);
	}
}

void MyEventHandler::onFrameStart()
{
	const float interval = nc::theApplication().interval();
	if (!pause_)
		angle_ += 2.0f * interval;

	const nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
	debugString_->clear();
	debugString_->formatAppend("batching: %s, batching with indices: %s", settings.batchingEnabled ? "on" : "off", settings.batchingWithIndices ? "on" : "off");
	debugText_->setString(*debugString_);

	const float width = nc::theApplication().width();
	const float height = nc::theApplication().height();
	const float numCharsInv = 1.0f / static_cast<float>(NumChars + 1);

	for (unsigned int i = 0; i < NumChars; i++)
	{
		characters_[i]->setPosition(width * numCharsInv * (i + 1), height * (0.5f + sinf(angle_ + 0.5f * i) * 0.15f));
		characters_[i]->setRotation(180.0f + sinf(angle_ + 0.5f * i) * -180.0f);
	}
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	checkClick(event.pointers[0].x, event.pointers[0].y);
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	pause_ = false;
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();
	nc::IDebugOverlay::DisplaySettings &overlaySettings = nc::theApplication().debugOverlaySettings();

	if (event.sym == nc::KeySym::B)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.sym == nc::KeySym::I)
		renderingSettings.batchingWithIndices = !renderingSettings.batchingWithIndices;
	else if (event.sym == nc::KeySym::H)
	{
		overlaySettings.showProfilerGraphs = !overlaySettings.showProfilerGraphs;
		overlaySettings.showInfoText = !overlaySettings.showInfoText;
	}
	else if (event.sym == nc::KeySym::BACKQUOTE)
		overlaySettings.showInterface = !overlaySettings.showInterface;
	else if (event.sym == nc::KeySym::P)
		pause_ = !pause_;
	else if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
	{
		const bool isSuspended = nc::theApplication().isSuspended();
		nc::theApplication().setSuspended(!isSuspended);
	}
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		checkClick(static_cast<float>(event.x), static_cast<float>(event.y));
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		pause_ = false;
}

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();
	nc::IDebugOverlay::DisplaySettings &overlaySettings = nc::theApplication().debugOverlaySettings();

	if (event.buttonName == nc::ButtonName::A)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.buttonName == nc::ButtonName::Y)
		renderingSettings.batchingWithIndices = !renderingSettings.batchingWithIndices;
	else if (event.buttonName == nc::ButtonName::BACK)
	{
		overlaySettings.showProfilerGraphs = !overlaySettings.showProfilerGraphs;
		overlaySettings.showInfoText = !overlaySettings.showInfoText;
	}
	else if (event.buttonName == nc::ButtonName::START)
		pause_ = !pause_;
	else if (event.buttonName == nc::ButtonName::GUIDE)
		nc::theApplication().quit();
}

void MyEventHandler::checkClick(float x, float y)
{
	const nc::Rectf debugTextRect = nc::Rectf::fromCenterAndSize(debugText_->absPosition(), debugText_->absSize());

	if (debugTextRect.contains(x, y))
	{
		nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
		const float xPos = x - debugTextRect.x;
		if (xPos <= debugTextRect.w * 0.33f)
			settings.batchingEnabled = !settings.batchingEnabled;
		else
			settings.batchingWithIndices = !settings.batchingWithIndices;
	}
	else
		pause_ = true;
}
