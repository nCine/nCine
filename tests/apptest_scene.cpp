#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_scene.h"
#include "Texture.h"
#include "Sprite.h"
#include "IFile.h" // for dataPath()

#ifdef __ANDROID__
	#include "AndroidApplication.h"
#else
	#include "Application.h"
#endif


nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::IInputManager::setHandler(this);

	for (unsigned int i = 0; i < NumTextures; i++)
	{
		angles_[i] = 0.0f;
	}

	nc::SceneNode &rootNode = nc::theApplication().rootNode();

#ifdef __ANDROID__
	textures_[0] = new nc::Texture((nc::IFile::dataPath() + "texture1.pkm").data()); // 145x121
	textures_[1] = new nc::Texture((nc::IFile::dataPath() + "texture2.pkm").data()); // 100x100
	textures_[2] = new nc::Texture((nc::IFile::dataPath() + "texture3.pkm").data()); // 96x96
	textures_[3] = new nc::Texture((nc::IFile::dataPath() + "texture4.pkm").data()); // 96x96
#else
	textures_[0] = new nc::Texture((nc::IFile::dataPath() + "textures/texture1.png").data());
	textures_[1] = new nc::Texture((nc::IFile::dataPath() + "textures/texture2.png").data());
	textures_[2] = new nc::Texture((nc::IFile::dataPath() + "textures/texture3.png").data());
	textures_[3] = new nc::Texture((nc::IFile::dataPath() + "textures/texture4.png").data());
#endif

	sprites_[0] = new nc::Sprite(textures_[0], 0, 0);
	sprites_[0]->setTexRect(nc::Recti(40, 0, static_cast<int>(sprites_[0]->width()) - 80, static_cast<int>(sprites_[0]->height()) - 30));
	sprites_[0]->setScale(0.75f);
	rootNode.addChildNode(sprites_[0]);

	for (unsigned int i = 1; i < NumSprites; i++)
	{
		sprites_[i] = new nc::Sprite(sprites_[i - 1], textures_[i % NumTextures], 0, 0);
		sprites_[i]->setScale(0.5f);
	}
}

void MyEventHandler::onFrameStart()
{
	float sine[NumTextures];
	float cosine[NumTextures];

	for (unsigned int i = 0; i < NumTextures; i++)
	{
		angles_[i] += (250 + 25 * i) * nc::theApplication().interval();
		sine[i] = sinf(angles_[i] * 0.01f);
		cosine[i] = cosf(2 * angles_[i] * 0.01f);
	}

	sprites_[0]->x = nc::theApplication().width() * 0.5f + sine[0] * 100.0f;
	sprites_[0]->y = nc::theApplication().height() * 0.5f + cosine[0] * 50.0f;

	for (unsigned int i = 1; i < NumSprites; i++)
	{
		sprites_[i]->x = sine[i % NumTextures] * 50;
		sprites_[i]->y = cosine[i % NumTextures] * 50;
	}
}

void MyEventHandler::onShutdown()
{
	delete sprites_[0]; // and all its children (the remaining sprites)
	for (unsigned int i = 0; i < NumTextures; i++)
	{
		delete textures_[i];
	}
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
