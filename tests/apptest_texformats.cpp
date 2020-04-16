#include "apptest_texformats.h"
#include <ncine/config.h>
#include <ncine/Application.h>
#include <ncine/SceneNode.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/TextNode.h>
#include <ncine/IGfxCapabilities.h>
#include "apptest_datapath.h"

#ifndef __ANDROID__
	#define WITH_PNG_FORMAT (1 && NCINE_WITH_PNG)
	#define WITH_WEBP_FORMAT (0 && NCINE_WITH_WEBP)
#else
	#define WITH_ETC1_FORMAT (1)
	#define WITH_ETC2_FORMAT (1)
	#define WITH_ATC_FORMAT (1)
	#define WITH_ASTC_FORMAT (1 && __ANDROID_API__ >= 21)
#endif

#define WITH_PVR_UNCOMPRESSED_FORMAT (0)
#define WITH_DDS_UNCOMPRESSED_FORMAT (0)
#define WITH_KTX_UNCOMPRESSED_FORMAT (0)
#define WITH_DDS_COMPRESSED_FORMAT (0)
#define WITH_MIPMAPPED_FORMATS (1)

namespace {

const float DeadScreenZone = 0.2f;
const float MaxScaleFactor = 8.0f;
const float MinScaleFactor = 1.0f / 32.0f;
const float VerticalTextPos = 0.45f;

const float ReleasedAxisThreshold = 0.45f;
const float PressedAxisThreshold = 0.85f;
unsigned char axesLeftStickPressed[4];

#ifdef __ANDROID__
const char *FontTextureFile = "DroidSans32_256.webp";
#else
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

}

int MyEventHandler::selected_ = 0;
int MyEventHandler::newSelection_ = 0;
float MyEventHandler::scale_ = 1.0f;
float MyEventHandler::newScale_ = 1.0f;

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

	nctl::String texPath = nc::fs::joinPath(nc::fs::dataPath(), "textures/testformats");
	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());

	const nc::IGfxCapabilities &gfxCaps = nc::theServiceLocator().gfxCapabilities();
	const int maxTextureSize = gfxCaps.value(nc::IGfxCapabilities::GLIntValues::MAX_TEXTURE_SIZE);
	FATAL_ASSERT_MSG_X(maxTextureSize >= 512, "Maximum device texture size is %d, which is less than 512 pixels", maxTextureSize);

#if WITH_PNG_FORMAT
	filenames_.pushBack("texture_512_RGB.png");
	filenames_.pushBack("texture_512_RGBA.png");
#endif

#if WITH_WEBP_FORMAT
	filenames_.pushBack("texture_512_RGB.webp");
	filenames_.pushBack("texture_512_RGBA.webp");
#endif

#if WITH_PVR_UNCOMPRESSED_FORMAT
	filenames_.pushBack("texture_512_RGB_888.pvr");
	filenames_.pushBack("texture_512_RGB_565.pvr");
	filenames_.pushBack("texture_512_RGBA_8888.pvr");
	filenames_.pushBack("texture_512_RGBA_4444.pvr");
#endif

#if WITH_DDS_UNCOMPRESSED_FORMAT
	filenames_.pushBack("texture_512_RGB_888.dds");
	// filenames_.pushBack("texture_512_RGB_565.dds"); // unsupported
	filenames_.pushBack("texture_512_RGBA_8888.dds");
#endif

#if WITH_KTX_UNCOMPRESSED_FORMAT
	filenames_.pushBack("texture_512_RGB_888.ktx"); // swapped channel
	filenames_.pushBack("texture_512_RGB_565.ktx");
	filenames_.pushBack("texture_512_RGBA_8888.ktx");
	filenames_.pushBack("texture_512_RGBA_4444.ktx");
#endif

#if WITH_DDS_COMPRESSED_FORMAT
	if (gfxCaps.hasExtension(nc::IGfxCapabilities::GLExtensions::EXT_TEXTURE_COMPRESSION_S3TC))
	{
		LOGI("S3TC textures are supported");
		filenames_.pushBack("texture_512_RGB_DXT1.dds");
		filenames_.pushBack("texture_512_RGBA_DXT3.dds");
		filenames_.pushBack("texture_512_RGBA_DXT5.dds");
	#if WITH_MIPMAPPED_FORMATS
		filenames_.pushBack("texture_512_RGB_DXT1_MIP.dds");
	#endif
	}
#endif

#if WITH_MIPMAPPED_FORMATS
	filenames_.pushBack("texture_512_RGB_888_MIP.pvr");
	filenames_.pushBack("texture_512_RGB_888_MIP.dds");
	filenames_.pushBack("texture_512_RGB_888_MIP.ktx");
#endif

#if WITH_ETC1_FORMAT
	if (gfxCaps.hasExtension(nc::IGfxCapabilities::GLExtensions::OES_COMPRESSED_ETC1_RGB8_TEXTURE))
	{
		LOGI("ETC1 textures are supported");
		filenames_.pushBack("texture_512_RGB_ETC1.ktx");
		filenames_.pushBack("texture_512_RGB_ETC1.dds");
		filenames_.pushBack("texture_512_RGB_ETC1.pvr");
		filenames_.pushBack("texture_512_RGB_ETC1.pkm");
	#if WITH_MIPMAPPED_FORMATS
		filenames_.pushBack("texture_512_RGB_ETC1_MIP.dds");
	#endif
	}
#endif

#if WITH_ETC2_FORMAT
	LOGI("ETC2 textures are supported");
	filenames_.pushBack("texture_512_RGBA_ETC2.ktx");
	filenames_.pushBack("texture_512_RGBA_ETC2.pvr");

	#if WITH_MIPMAPPED_FORMATS
	filenames_.pushBack("texture_512_RGBA_ETC2_MIP.ktx");
	filenames_.pushBack("texture_512_RGBA_ETC2_MIP.pvr");
	#endif
#endif

#if WITH_ATC_FORMAT
	if (gfxCaps.hasExtension(nc::IGfxCapabilities::GLExtensions::AMD_COMPRESSED_ATC_TEXTURE))
	{
		LOGI("ATC textures are supported");
		filenames_.pushBack("texture_512_RGB_ATC.dds");
		filenames_.pushBack("texture_512_RGBA_ATC_explicit.dds");
		filenames_.pushBack("texture_512_RGBA_ATC_interpolated.dds");
	}
#endif

#if WITH_ASTC_FORMAT
	if (gfxCaps.hasExtension(nc::IGfxCapabilities::GLExtensions::KHR_TEXTURE_COMPRESSION_ASTC_LDR))
	{
		LOGI("ASTC textures are supported");
		filenames_.pushBack("texture_512_RGB_ASTC8x8.ktx");
		filenames_.pushBack("texture_512_RGB_ASTC8x8.pvr");
		filenames_.pushBack("texture_512_RGBA_ASTC8x8.ktx");
		filenames_.pushBack("texture_512_RGBA_ASTC8x8.pvr");
	#if WITH_MIPMAPPED_FORMATS
		filenames_.pushBack("texture_512_RGBA_ASTC8x8_MIP.ktx");
		filenames_.pushBack("texture_512_RGBA_ASTC8x8_MIP.pvr");
	#endif
	}
#endif

	FATAL_ASSERT_MSG(!filenames_.isEmpty(), "No texture file names to load");

	dummy_ = nctl::makeUnique<nc::SceneNode>(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	textNode_ = nctl::makeUnique<nc::TextNode>(dummy_.get(), font_.get());

	for (unsigned int i = 0; i < filenames_.size(); i++)
	{
		textures_.pushBack(nctl::makeUnique<nc::Texture>((nc::fs::absoluteJoinPath(texPath, filenames_[i])).data()));
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(dummy_.get(), textures_[i].get()));
		sprites_.back()->setEnabled(false);
	}

	sprites_[selected_]->setEnabled(true);
	textNode_->setString(filenames_[selected_]);
	textNode_->setPosition(0.0f, nc::theApplication().height() * VerticalTextPos - textNode_->height() * 0.5f);
}

#ifdef __ANDROID__
void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	handleCoordInput(event.pointers[0].x, event.pointers[0].y);
}
#endif

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		handleCoordInput(static_cast<float>(event.x), static_cast<float>(event.y));
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::D || event.sym == nc::KeySym::RIGHT)
		handleInput(Direction::RIGHT);
	else if (event.sym == nc::KeySym::A || event.sym == nc::KeySym::LEFT)
		handleInput(Direction::LEFT);
	else if (event.sym == nc::KeySym::W || event.sym == nc::KeySym::UP)
		handleInput(Direction::UP);
	else if (event.sym == nc::KeySym::S || event.sym == nc::KeySym::DOWN)
		handleInput(Direction::DOWN);
	else if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
	{
		const bool isSuspended = nc::theApplication().isSuspended();
		nc::theApplication().setSuspended(!isSuspended);
	}
}

void MyEventHandler::handleInput(Direction direction)
{
	switch (direction)
	{
		case Direction::RIGHT:
			newSelection_ = selected_ + 1;
			if (newSelection_ > static_cast<int>(filenames_.size() - 1))
				newSelection_ = 0;
			break;
		case Direction::LEFT:
			newSelection_ = selected_ - 1;
			if (newSelection_ < 0)
				newSelection_ = filenames_.size() - 1;
			break;
		case Direction::UP:
			newScale_ = scale_ * 2.0f;
			if (newScale_ > MaxScaleFactor)
				newScale_ = MaxScaleFactor;
			break;
		case Direction::DOWN:
			newScale_ = scale_ * 0.5f;
			if (newScale_ < MinScaleFactor)
				newScale_ = MinScaleFactor;
			break;
	}

	if (newSelection_ != selected_)
	{
		sprites_[selected_]->setEnabled(false);
		sprites_[newSelection_]->setEnabled(true);
		textNode_->setString(filenames_[newSelection_]);
		textNode_->setPosition(0.0f, nc::theApplication().height() * VerticalTextPos - textNode_->height() * 0.5f);
		selected_ = newSelection_;
	}

	if (newScale_ != scale_)
	{
		for (unsigned int i = 0; i < sprites_.size(); i++)
			sprites_[i]->setScale(newScale_);
		scale_ = newScale_;
	}
}

void MyEventHandler::handleCoordInput(float x, float y)
{
	if (x > nc::theApplication().width() * (0.5f + DeadScreenZone))
		handleInput(Direction::RIGHT);
	else if (x < nc::theApplication().width() * (0.5f - DeadScreenZone))
		handleInput(Direction::LEFT);

	if (y > nc::theApplication().height() * (0.5f + DeadScreenZone))
		handleInput(Direction::UP);
	else if (y < nc::theApplication().height() * (0.5f - DeadScreenZone))
		handleInput(Direction::DOWN);
}

void MyEventHandler::onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event)
{
	if (event.axisName == nc::AxisName::LX)
	{
		float x = event.value;
		if (x > PressedAxisThreshold && axesLeftStickPressed[0] == false)
		{
			handleInput(Direction::RIGHT);
			axesLeftStickPressed[0] = true;
		}
		else if (x > 0.0f && x < ReleasedAxisThreshold)
			axesLeftStickPressed[0] = false;
		else if (x < -PressedAxisThreshold && axesLeftStickPressed[1] == false)
		{
			handleInput(Direction::LEFT);
			axesLeftStickPressed[1] = true;
		}
		else if (x < 0.0f && x > -ReleasedAxisThreshold)
			axesLeftStickPressed[1] = false;
	}
	else if (event.axisName == nc::AxisName::LY)
	{
		float y = -event.value;
		if (y > PressedAxisThreshold && axesLeftStickPressed[2] == false)
		{
			handleInput(Direction::UP);
			axesLeftStickPressed[2] = true;
		}
		else if (y > 0.0f && y < ReleasedAxisThreshold)
			axesLeftStickPressed[2] = false;
		else if (y < -PressedAxisThreshold && axesLeftStickPressed[3] == false)
		{
			handleInput(Direction::DOWN);
			axesLeftStickPressed[3] = true;
		}
		else if (y < 0.0f && y > -ReleasedAxisThreshold)
			axesLeftStickPressed[3] = false;
	}
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	axesLeftStickPressed[0] = false;
	axesLeftStickPressed[1] = false;
	axesLeftStickPressed[2] = false;
	axesLeftStickPressed[3] = false;
}
