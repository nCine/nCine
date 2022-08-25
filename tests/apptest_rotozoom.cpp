#include "apptest_rotozoom.h"
#include <ncine/Application.h>
#include <ncine/SceneNode.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
const char *TextureFile = "megatexture_256_ETC2.ktx";
#else
const char *FontTextureFile = "DroidSans32_256.png";
const char *TextureFile = "megatexture_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

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
	pauseSprites_ = false;
	pauseGroup_ = false;
	angleSprites_ = 0.0f;
	angleGroup_ = 0.0f;
	nctl::StaticArray<nc::Recti, NumTextures> texRects;

	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());

	debugText_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get());
	debugText_->setPosition((nc::theApplication().width() - debugText_->width()) * 0.5f,
	                        nc::theApplication().height() - debugText_->lineHeight() * 0.5f * 2.0f);
	debugText_->setColor(255, 255, 0, 255);
	debugText_->setAlignment(nc::TextNode::Alignment::CENTER);

	megaTexture_ = nctl::makeUnique<nc::Texture>(prefixDataPath("textures", TextureFile).data());

	texRects.pushBack(nc::Recti(0, 0, 128, 128));
	texRects.pushBack(nc::Recti(128, 0, 128, 128));
	texRects.pushBack(nc::Recti(0, 128, 128, 128));
	texRects.pushBack(nc::Recti(128, 128, 128, 128));

	dummy_ = nctl::makeUnique<nc::SceneNode>(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	parent_ = nctl::makeUnique<nc::SceneNode>(dummy_.get(), 0.0f, 0.0f);
	parent_->setScale(0.75f);

	unsigned int index = 0;
	for (int i = static_cast<int>(-NumRowSprites * 0.5f); i < static_cast<int>(NumRowSprites * 0.5f); i++)
	{
		for (int j = static_cast<int>(-NumColSprites * 0.5f); j < static_cast<int>(NumColSprites * 0.5f); j++)
		{
			sprites_.pushBack(nctl::makeUnique<nc::Sprite>(parent_.get(), megaTexture_.get(), i * 128.0f, j * 128.0f));
			sprites_.back()->setTexRect(texRects[index % NumTextures]);
			sprites_.back()->setScale(0.75f);
			index++;
		}
	}
}

void MyEventHandler::onFrameStart()
{
	const nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
	debugString_.clear();
	debugString_.formatAppend("batching: %s, culling: %s", settings.batchingEnabled ? "on" : "off", settings.cullingEnabled ? "on" : "off");
	debugText_->setString(debugString_.data());

	if (pauseSprites_ == false)
	{
		angleSprites_ += 2.5f * nc::theApplication().interval();
		for (unsigned int i = 0; i < NumColSprites * NumRowSprites; i++)
			sprites_[i]->setRotation(-angleSprites_ * 8.0f);
	}

	if (pauseGroup_ == false)
	{
		angleGroup_ += 2.5f * nc::theApplication().interval();
		const float sine = sinf(angleGroup_);
		const float cosine = cosf(angleGroup_);

		parent_->setPositionX(nc::theApplication().width() * 0.5f + sine * 100.0f);
		parent_->setPositionY(nc::theApplication().height() * 0.5f + cosine * 150.0f);
		parent_->setRotation(angleGroup_ * 8.0f);
		parent_->setScale(((sine * 0.15f) + 1.0f) * 0.5f);

		dummy_->setPositionX((sine + 1.0f) * 50.0f);
		dummy_->setPositionY((cosine + 1.0f) * 50.0f);
		dummy_->setRotation(sine * 10.0f);
		dummy_->setScale(((cosine * 0.1f) + 1.0f) * 0.75f);
	}
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	if (event.pointers[0].x < nc::theApplication().width() * 0.5f)
		pauseSprites_ = true;
	else
		pauseGroup_ = true;
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	pauseSprites_ = false;
	pauseGroup_ = false;
}

void MyEventHandler::onPointerDown(const nc::TouchEvent &event)
{
	if (event.count == 2)
	{
		if (event.pointers[1].x < nc::theApplication().width() * 0.5f)
			pauseSprites_ = true;
		else
			pauseGroup_ = true;
	}
}

void MyEventHandler::onPointerUp(const nc::TouchEvent &event)
{
	if (event.count == 2)
	{
		const float halfWidth = nc::theApplication().width() * 0.5f;
		if (event.pointers[1].x < halfWidth)
			pauseSprites_ = false;
		else
			pauseGroup_ = false;
	}
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();

	if (event.sym == nc::KeySym::B)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.sym == nc::KeySym::C)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		checkClick(static_cast<float>(event.x), static_cast<float>(event.y));
	else if (event.isRightButton())
		pauseGroup_ = true;
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		pauseSprites_ = false;
	else if (event.isRightButton())
		pauseGroup_ = false;
}

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();

	if (event.buttonName == nc::ButtonName::A)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.buttonName == nc::ButtonName::B)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
	else if (event.buttonName == nc::ButtonName::LBUMPER)
		pauseSprites_ = !pauseSprites_;
	else if (event.buttonName == nc::ButtonName::RBUMPER)
		pauseGroup_ = !pauseGroup_;
	else if (event.buttonName == nc::ButtonName::START)
	{
		pauseSprites_ = !pauseSprites_;
		pauseGroup_ = !pauseGroup_;
	}
	else if (event.buttonName == nc::ButtonName::GUIDE)
		nc::theApplication().quit();
}

void MyEventHandler::checkClick(float x, float y)
{
	const nc::Rectf debugTextRect = nc::Rectf::fromCenterSize(debugText_->absPosition(), debugText_->absSize());

	if (debugTextRect.contains(x, y))
	{
		nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
		const float xPos = x - debugTextRect.x;
		if (xPos <= debugTextRect.w * 0.55f)
			settings.batchingEnabled = !settings.batchingEnabled;
		else
			settings.cullingEnabled = !settings.cullingEnabled;
	}
	else
		pauseSprites_ = true;
}
