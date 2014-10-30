#include "apptest_particles.h"
#include "Application.h"
#include "Texture.h"
#include "ParticleSystem.h"
#include "IInputManager.h"
#ifdef __ANDROID__
	#include "AndroidInputManager.h"
#endif

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::IInputManager::setHandler(this);
	nc::SceneNode &rootNode = nc::Application::rootNode();

#ifdef __ANDROID__
	nc::AndroidInputManager::enableAccelerometer(true);
//	texture_ = new nc::Texture("/sdcard/ncine/smoke_128.dds"); // Adreno SD
	texture_ = new nc::Texture("/sdcard/ncine/smoke2_256_8888.pvr"); // Mali HD
#else
//	texture_ = new nc::Texture("textures/smoke_256.webp");
//	texture_ = new nc::Texture("textures/smoke_256_4444.pvr");
	texture_ = new nc::Texture("textures/smoke_256.png");
#endif

	particleSystem_ = new nc::ParticleSystem(&rootNode, NumParticles, texture_, texture_->rect());
	particleSystem_->setPosition(nc::Application::width() * 0.5f, nc::Application::height() * 0.33f);

//	particleSystem_->addAffector(new AccelerationAffector(0.000025f, 0.0f));
	nc::ColorAffector *colAffector = new nc::ColorAffector();
	colAffector->addColorStep(0.0f, nc::Color(0.86f, 0.39f, 0.0f, 0.7f)); // 0.05
	colAffector->addColorStep(0.65f, nc::Color(0.86f, 0.59f, 0.0f, 0.75f)); // 0.55
	colAffector->addColorStep(0.7f, nc::Color(0.86f, 0.7f, 0.0f, 0.6)); // 0.295
	colAffector->addColorStep(1.0f, nc::Color(0.0f, 0.0f, 1.0f, 0.85)); // 0.59
	particleSystem_->addAffector(colAffector);
	nc::SizeAffector *sizeAffector = new nc::SizeAffector(0.45f); // 0.25
	sizeAffector->addSizeStep(0.0f, 0.01f);
	sizeAffector->addSizeStep(0.7f, 1.6f);
	sizeAffector->addSizeStep(1.0f, 0.4f);
	particleSystem_->addAffector(sizeAffector);
	emitVector_.set(0.0f, 350.0f);

	emitTimer_ = new nc::Timer();
	emitTimer_->start();
}

void MyEventHandler::onFrameStart()
{
	if (emitTimer_->interval() > 0.085f) // 0.150f
	{
		emitTimer_->start();
		particleSystem_->emitParticles(3, 1.0f, emitVector_); // (25, 3.0f, Vector2f(0.0f, 100.0f))
	}
}

void MyEventHandler::onFrameEnd()
{
#ifndef __ANDROID__
	const nc::KeyboardState &keyState = nc::Application::inputManager().keyboardState();

	if (keyState.isKeyDown(nc::KEY_RIGHT))
	{
		particleSystem_->x += 0.1f * nc::Application::interval();
	}
	else if (keyState.isKeyDown(nc::KEY_LEFT))
	{
		particleSystem_->x -= 0.1f * nc::Application::interval();
	}
	else if (keyState.isKeyDown(nc::KEY_UP))
	{
		particleSystem_->y += 0.1f * nc::Application::interval();
	}
	else if (keyState.isKeyDown(nc::KEY_DOWN))
	{
		particleSystem_->y -= 0.1f * nc::Application::interval();
	}
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
	particleSystem_->x = event.x;
	particleSystem_->y = event.y;
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	particleSystem_->x = event.x;
	particleSystem_->y = event.y;

	if (event.count > 1)
	{
		emitVector_.x = (event.x2 - particleSystem_->x) * 2.5f;
		emitVector_.y = (event.y2 - particleSystem_->y) * 2.5f;
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
		nc::Application::quit();
	}
	else if (event.sym == nc::KEY_SPACE)
	{
		nc::Application::togglePause();
	}
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
	{
		particleSystem_->x = event.x;
		particleSystem_->y = event.y;
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (state.isLeftButtonDown())
	{
		particleSystem_->x = state.x;
		particleSystem_->y = state.y;
	}
	else if (state.isRightButtonDown())
	{
		emitVector_.x = (state.x - particleSystem_->x) * 2.5f;
		emitVector_.y = (state.y - particleSystem_->y) * 2.5f;
	}
}
#endif
