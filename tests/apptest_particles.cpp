#include "apptest_particles.h"
#include "Application.h"
#include "Texture.h"
#include "ParticleSystem.h"
#include "Timer.h"
#include "IFile.h" // for dataPath()
#include "apptest_datapath.h"
#include "apptest_joymapping.h"

#ifdef __ANDROID__
	#include "AndroidApplication.h"
#endif

namespace {

#ifdef __ANDROID__
	const char *TextureFile = "smoke_256.webp";
#else
	const char *TextureFile = "smoke_256.png";
	const float KeySpeed = 200.0f;
#endif

const float JoySpeed = 200.0f;

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

#ifdef __ANDROID__
	nc::AndroidApplication &application = static_cast<nc::AndroidApplication &>(nc::theApplication());
	application.enableAccelerometer(true);
#endif

	texture_ = new nc::Texture((nc::IFile::dataPath() + "textures/" + TextureFile).data());
	particleSystem_ = new nc::ParticleSystem(&rootNode, NumParticles, texture_, texture_->rect());
	particleSystem_->setPosition(nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.33f);

	//particleSystem_->addAffector(new nc::AccelerationAffector(0.2f, 0.0f));
	nc::ColorAffector *colAffector = new nc::ColorAffector();
	colAffector->addColorStep(0.0f, nc::Color(0.86f, 0.39f, 0.0f, 0.75f));
	colAffector->addColorStep(0.65f, nc::Color(0.86f, 0.59f, 0.0f, 0.8f));
	colAffector->addColorStep(0.7f, nc::Color(0.86f, 0.7f, 0.0f, 0.65f));
	colAffector->addColorStep(1.0f, nc::Color(0.0f, 0.0f, 1.0f, 0.9f));
	particleSystem_->addAffector(colAffector);
	nc::SizeAffector *sizeAffector = new nc::SizeAffector(0.45f);
	sizeAffector->addSizeStep(0.0f, 0.01f);
	sizeAffector->addSizeStep(0.7f, 1.7f);
	sizeAffector->addSizeStep(1.0f, 0.4f);
	particleSystem_->addAffector(sizeAffector);
	emitVector_.set(0.0f, 350.0f);

	emitTimer_ = new nc::Timer();
	emitTimer_->start();

	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}

void MyEventHandler::onFrameStart()
{
	if (emitTimer_->interval() > 0.085f)
	{
		emitTimer_->start();
		particleSystem_->emitParticles(3, 1.0f, emitVector_);
	}

	particleSystem_->move(joyVectorLeft_ * JoySpeed * nc::theApplication().interval());
	emitVector_ += joyVectorRight_ * JoySpeed * nc::theApplication().interval();

#ifndef __ANDROID__
	const nc::KeyboardState &keyState = nc::theApplication().inputManager().keyboardState();

	if (keyState.isKeyDown(nc::KEY_D)) { particleSystem_->x += KeySpeed * nc::theApplication().interval(); }
	else if (keyState.isKeyDown(nc::KEY_A)) { particleSystem_->x -= KeySpeed * nc::theApplication().interval(); }
	if (keyState.isKeyDown(nc::KEY_W)) { particleSystem_->y += KeySpeed * nc::theApplication().interval(); }
	else if (keyState.isKeyDown(nc::KEY_S)) { particleSystem_->y -= KeySpeed * nc::theApplication().interval(); }

	if (keyState.isKeyDown(nc::KEY_RIGHT)) { emitVector_.x += KeySpeed * nc::theApplication().interval(); }
	else if (keyState.isKeyDown(nc::KEY_LEFT)) { emitVector_.x -= KeySpeed * nc::theApplication().interval(); }
	if (keyState.isKeyDown(nc::KEY_UP)) { emitVector_.y += KeySpeed * nc::theApplication().interval(); }
	else if (keyState.isKeyDown(nc::KEY_DOWN)) { emitVector_.y -= KeySpeed * nc::theApplication().interval(); }
#endif
}

void MyEventHandler::onShutdown()
{
	delete emitTimer_;
	delete particleSystem_;
	delete texture_;
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	particleSystem_->x = event.pointers[0].x;
	particleSystem_->y = event.pointers[0].y;
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	particleSystem_->x = event.pointers[0].x;
	particleSystem_->y = event.pointers[0].y;

	if (event.count > 1)
	{
		emitVector_.x = (event.pointers[1].x - particleSystem_->x) * 2.5f;
		emitVector_.y = (event.pointers[1].y - particleSystem_->y) * 2.5f;
	}
}

void MyEventHandler::onAcceleration(const nc::AccelerometerEvent &event)
{
	particleSystem_->x += event.y * 0.75f;
	particleSystem_->y += -event.x * 0.75f;
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

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
	{
		particleSystem_->x = static_cast<float>(event.x);
		particleSystem_->y = static_cast<float>(event.y);
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (state.isLeftButtonDown())
	{
		particleSystem_->x = static_cast<float>(state.x);
		particleSystem_->y = static_cast<float>(state.y);
	}
	else if (state.isRightButtonDown())
	{
		emitVector_.x = (state.x - particleSystem_->x) * 2.5f;
		emitVector_.y = (state.y - particleSystem_->y) * 2.5f;
	}
}
#endif

void MyEventHandler::onJoyAxisMoved(const nc::JoyAxisEvent &event)
{
	if (isAxis(event, AXIS_LX)) { joyVectorLeft_.x = normValue(event, AXIS_LX); }
	else if (isAxis(event, AXIS_LY)) { joyVectorLeft_.y = -normValue(event, AXIS_LY); }

	if (isAxis(event, AXIS_RX)) { joyVectorRight_.x = normValue(event, AXIS_RX); }
	else if (isAxis(event, AXIS_RY)) { joyVectorRight_.y = -normValue(event, AXIS_RY); }

	deadZoneNormalize(joyVectorLeft_, LeftStickDeadZone);
	deadZoneNormalize(joyVectorRight_, RightStickDeadZone);
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}
