#include "apptest_texformats.h"
#include "Application.h"
#include "SceneNode.h"
#include "Texture.h"
#include "Sprite.h"
#include "TextNode.h"
#include "IGfxCapabilities.h"
#include "IFile.h" // for dataPath()

#ifndef __ANDROID__
	#define WITH_PNG_FORMAT (1)
	#define WITH_WEBP_FORMAT (0)
#else
	#define WITH_ETC1_FORMAT (1)
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

#ifdef __ANDROID__
const char* FontTextureFile = "DroidSans32_256_8888.ktx";
#else
const char* FontTextureFile = "DroidSans32_256.png";
#endif
const char* FontFntFile = "DroidSans32_256.fnt";

}

int MyEventHandler::selected_ = 0;
int MyEventHandler::newSelection_ = 0;
float MyEventHandler::scale_ = 1.0f;
float MyEventHandler::newScale_ = 1.0f;

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	nc::String texPath = nc::IFile::dataPath() + "textures/testformats/";
	font_ = new nc::Font((nc::IFile::dataPath() + "fonts/" + FontTextureFile).data(),
						 (nc::IFile::dataPath() + "fonts/" + FontFntFile).data());

	const nc::IGfxCapabilities &gfxCaps = nc::theServiceLocator().gfxCapabilities();
	if (gfxCaps.value(nc::IGfxCapabilities::MAX_TEXTURE_SIZE) < 512)
	{
		LOGF("Maximum device texture size is less than 512 pixels");
		exit(EXIT_FAILURE);
	}

#if WITH_PNG_FORMAT
	filenames_.insertBack("texture_512_RGB.png");
	filenames_.insertBack("texture_512_RGBA.png");
#endif

#if WITH_WEBP_FORMAT
	filenames_.insertBack("texture_512_RGB.webp");
	filenames_.insertBack("texture_512_RGBA.webp");
#endif

#if WITH_PVR_UNCOMPRESSED_FORMAT
	filenames_.insertBack("texture_512_RGB_888.pvr");
	filenames_.insertBack("texture_512_RGB_565.pvr");
	filenames_.insertBack("texture_512_RGBA_8888.pvr");
	filenames_.insertBack("texture_512_RGBA_4444.pvr");
#endif

#if WITH_DDS_UNCOMPRESSED_FORMAT
	filenames_.insertBack("texture_512_RGB_888.dds");
	//filenames_.insertBack("texture_512_RGB_565.dds"); // unsupported
	filenames_.insertBack("texture_512_RGBA_8888.dds");
#endif

#if WITH_KTX_UNCOMPRESSED_FORMAT
	filenames_.insertBack("texture_512_RGB_888.ktx"); // swapped channel
	filenames_.insertBack("texture_512_RGB_565.ktx");
	filenames_.insertBack("texture_512_RGBA_8888.ktx");
	filenames_.insertBack("texture_512_RGBA_4444.ktx");
#endif

#if WITH_DDS_COMPRESSED_FORMAT
	if (gfxCaps.hasExtension(nc::IGfxCapabilities::EXT_TEXTURE_COMPRESSION_S3TC))
	{
		LOGI("S3TC textures are supported");
		filenames_.insertBack("texture_512_RGB_DXT1.dds");
		filenames_.insertBack("texture_512_RGBA_DXT3.dds");
		filenames_.insertBack("texture_512_RGBA_DXT5.dds");
	#if WITH_MIPMAPPED_FORMATS
		filenames_.insertBack("texture_512_RGB_DXT1_MIP.dds");
	#endif
	}
#endif

#if WITH_MIPMAPPED_FORMATS
	filenames_.insertBack("texture_512_RGB_888_MIP.pvr");
	filenames_.insertBack("texture_512_RGB_888_MIP.dds");
	filenames_.insertBack("texture_512_RGB_888_MIP.ktx");
#endif


#if WITH_ETC1_FORMAT
	if (gfxCaps.hasExtension(nc::IGfxCapabilities::OES_COMPRESSED_ETC1_RGB8_TEXTURE))
	{
		LOGI("ETC1 textures are supported");
		filenames_.insertBack("texture_512_RGB_ETC1.ktx");
		filenames_.insertBack("texture_512_RGB_ETC1.dds");
		filenames_.insertBack("texture_512_RGB_ETC1.pvr");
		filenames_.insertBack("texture_512_RGB_ETC1.pkm");
	#if WITH_MIPMAPPED_FORMATS
		filenames_.insertBack("texture_512_RGB_ETC1_MIP.dds");
	#endif
	}
#endif

#if WITH_ATC_FORMAT
	if (gfxCaps.hasExtension(nc::IGfxCapabilities::AMD_COMPRESSED_ATC_TEXTURE))
	{
		LOGI("ATC textures are supported");
		filenames_.insertBack("texture_512_RGB_ATC.dds");
		filenames_.insertBack("texture_512_RGBA_ATC_explicit.dds");
		filenames_.insertBack("texture_512_RGBA_ATC_interpolated.dds");
	}
#endif

#if WITH_ASTC_FORMAT
	if (gfxCaps.hasExtension(nc::IGfxCapabilities::KHR_TEXTURE_COMPRESSION_ASTC_LDR))
	{
		LOGI("ASTC textures are supported");
		filenames_.insertBack("texture_512_RGB_ASTC8x8.ktx");
		filenames_.insertBack("texture_512_RGB_ASTC8x8.pvr");
		filenames_.insertBack("texture_512_RGBA_ASTC8x8.ktx");
		filenames_.insertBack("texture_512_RGBA_ASTC8x8.pvr");
	#if WITH_MIPMAPPED_FORMATS
		filenames_.insertBack("texture_512_RGBA_ASTC8x8_MIP.ktx");
		filenames_.insertBack("texture_512_RGBA_ASTC8x8_MIP.pvr");
	#endif
	}
#endif

	if (filenames_.isEmpty())
	{
		LOGF("No texture file names to load");
		exit(EXIT_FAILURE);
	}

	dummy_ = new nc::SceneNode(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	textNode_ = new nc::TextNode(dummy_, font_);

	for (unsigned int i = 0; i < filenames_.size(); i++)
	{
		textures_[i] = new nc::Texture((texPath + filenames_[i]).data());
		sprites_[i] = new nc::Sprite(dummy_, textures_[i]);
		sprites_[i]->enableDraw(false);
	}

	sprites_[selected_]->enableDraw(true);
	textNode_->setString(filenames_[selected_]);
	textNode_->setPosition(-textNode_->width() * 0.5f, nc::theApplication().height() * VerticalTextPos);
}

void MyEventHandler::onShutdown()
{
	for (unsigned int i = 0; i < sprites_.size(); i++)
	{
		delete sprites_[i];
	}

	for (unsigned int i = 0; i < textures_.size(); i++)
	{
		delete textures_[i];
	}

	delete textNode_;
	delete dummy_; // and all its children
	delete font_;
}

#ifdef __ANDROID__
void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	handleInput(event.x, event.y);
}
#else
void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
	{
		handleInput(static_cast<float>(event.x), static_cast<float>(event.y));
	}
}

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

void MyEventHandler::handleInput(float x, float y)
{
	if (x > nc::theApplication().width() * (0.5f + DeadScreenZone))
	{
		newSelection_ = selected_ + 1;
		if (newSelection_ > static_cast<int>(filenames_.size() - 1))
		{
			newSelection_ = 0;
		}
	}
	else if (x < nc::theApplication().width() * (0.5f - DeadScreenZone))
	{
		newSelection_ = selected_ - 1;
		if (newSelection_ < 0)
		{
			newSelection_ = filenames_.size() - 1;
		}
	}

	if (y > nc::theApplication().height() * (0.5f + DeadScreenZone))
	{
		newScale_ = scale_ * 2.0f;
		if (newScale_ > MaxScaleFactor)
		{
			newScale_ = MaxScaleFactor;
		}
	}
	else if (y < nc::theApplication().height() * (0.5f - DeadScreenZone))
	{
		newScale_ = scale_ * 0.5f;
		if (newScale_ < MinScaleFactor)
		{
			newScale_ = MinScaleFactor;
		}
	}

	if (newSelection_ != selected_)
	{
		sprites_[selected_]->enableDraw(false);
		sprites_[newSelection_]->enableDraw(true);
		textNode_->setString(filenames_[newSelection_]);
		textNode_->setPosition(-textNode_->width() * 0.5f, nc::theApplication().height() * VerticalTextPos);
		selected_ = newSelection_;
	}

	if (newScale_ != scale_)
	{
		for (unsigned int i = 0; i < sprites_.size(); i++)
		{
			sprites_[i]->setScale(newScale_);
		}
		scale_ = newScale_;
	}
}
