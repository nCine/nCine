#include "apptest_rotozoom.h"
#include <ncine/Application.h>
#include <ncine/SceneNode.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *TextureFile = "megatexture_256_ETC2.ktx";
#else
const char *TextureFile = "megatexture_256.png";
#endif

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
	pause_ = false;
	angle_ = 0.0f;
	nctl::StaticArray<nc::Recti, NumTextures> texRects;

	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	megaTexture_ = nctl::makeUnique<nc::Texture>(prefixDataPath("textures", TextureFile).data());

	texRects.pushBack(nc::Recti(0, 0, 128, 128));
	texRects.pushBack(nc::Recti(128, 0, 128, 128));
	texRects.pushBack(nc::Recti(0, 128, 128, 128));
	texRects.pushBack(nc::Recti(128, 128, 128, 128));

	dummy_ = nctl::makeUnique<nc::SceneNode>(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	dummy_->setScale(0.75f);

	unsigned int index = 0;
	for (int i = static_cast<int>(-NumRowSprites * 0.5f); i < static_cast<int>(NumRowSprites * 0.5f); i++)
	{
		for (int j = static_cast<int>(-NumColSprites * 0.5f); j < static_cast<int>(NumColSprites * 0.5f); j++)
		{
			sprites_.pushBack(nctl::makeUnique<nc::Sprite>(dummy_.get(), megaTexture_.get(), i * 128.0f, j * 128.0f));
			sprites_.back()->setTexRect(texRects[index % NumTextures]);
			sprites_.back()->setScale(0.75f);
			index++;
		}
	}
}

void MyEventHandler::onFrameStart()
{
	if (pause_ == false)
		angle_ += 2.5f * nc::theApplication().interval();
	const float sine = sinf(angle_);
	const float cosine = cosf(angle_);

	dummy_->x = nc::theApplication().width() * 0.5f + sine * 100.0f;
	dummy_->y = nc::theApplication().height() * 0.5f + cosine * 150.0f;
	dummy_->setRotation(angle_ * 8.0f);
	dummy_->setScale(((sine * 0.15f) + 1.0f) * 0.5f);

	for (unsigned int i = 0; i < NumColSprites * NumRowSprites; i++)
		sprites_[i]->setRotation(-angle_ * 8.0f);

	nc::theApplication().rootNode().x = (sine + 1.0f) * 50.0f;
	nc::theApplication().rootNode().y = (cosine + 1.0f) * 50.0f;
	nc::theApplication().rootNode().setRotation(sine * 10.0f);
	nc::theApplication().rootNode().setScale(((cosine * 0.1f) + 1.0f) * 0.75f);
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	pause_ = true;
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	pause_ = false;
}
#endif

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		pause_ = true;
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		pause_ = false;
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
