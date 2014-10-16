#include "apptest_particles.h"
#include "ncApplication.h"
#include "ncTexture.h"
#include "ncParticleSystem.h"
#include "ncIInputManager.h"
#ifdef __ANDROID__
	#include "ncAndroidInputManager.h"
#endif

ncIAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	ncIInputManager::setHandler(this);
	ncSceneNode &rootNode = ncApplication::rootNode();

#ifdef __ANDROID__
	ncAndroidInputManager::enableAccelerometer(true);
//	texture_ = new ncTexture("/sdcard/ncine/smoke_128.dds"); // Adreno SD
	texture_ = new ncTexture("/sdcard/ncine/smoke2_256_8888.pvr"); // Mali HD
#else
//	texture_ = new ncTexture("textures/smoke_256.webp");
//	texture_ = new ncTexture("textures/smoke_256_4444.pvr");
	texture_ = new ncTexture("textures/smoke_256.png");
#endif

	particleSystem_ = new ncParticleSystem(&rootNode, NumParticles, texture_, texture_->rect());
	particleSystem_->setPosition(ncApplication::width() * 0.5f, ncApplication::height() * 0.33f);

//	particleSystem_->addAffector(new ncAccelerationAffector(0.000025f, 0.0f));
	ncColorAffector *colAffector = new ncColorAffector();
	colAffector->addColorStep(0.0f, ncColor(0.86f, 0.39f, 0.0f, 0.7f)); // 0.05
	colAffector->addColorStep(0.65f, ncColor(0.86f, 0.59f, 0.0f, 0.75f)); // 0.55
	colAffector->addColorStep(0.7f, ncColor(0.86f, 0.7f, 0.0f, 0.6)); // 0.295
	colAffector->addColorStep(1.0f, ncColor(0.0f, 0.0f, 1.0f, 0.85)); // 0.59
	particleSystem_->addAffector(colAffector);
	ncSizeAffector *sizeAffector = new ncSizeAffector(0.45f); // 0.25
	sizeAffector->addSizeStep(0.0f, 0.01f);
	sizeAffector->addSizeStep(0.7f, 1.6f);
	sizeAffector->addSizeStep(1.0f, 0.4f);
	particleSystem_->addAffector(sizeAffector);
	emitVector_.set(0.0f, 350.0f);

	emitTimer_ = new ncTimer();
	emitTimer_->start();
}

void MyEventHandler::onFrameStart()
{
	if (emitTimer_->interval() > 0.085f) // 0.150f
	{
		emitTimer_->start();
		particleSystem_->emitParticles(3, 1.0f, emitVector_); // (25, 3.0f, ncVector2f(0.0f, 100.0f))
	}
}

void MyEventHandler::onFrameEnd()
{
#ifndef __ANDROID__
	const ncKeyboardState &keyState = ncApplication::inputManager().keyboardState();

	if (keyState.isKeyDown(NCKEY_RIGHT))
	{
		particleSystem_->x += 0.1f * ncApplication::interval();
	}
	else if (keyState.isKeyDown(NCKEY_LEFT))
	{
		particleSystem_->x -= 0.1f * ncApplication::interval();
	}
	else if (keyState.isKeyDown(NCKEY_UP))
	{
		particleSystem_->y += 0.1f * ncApplication::interval();
	}
	else if (keyState.isKeyDown(NCKEY_DOWN))
	{
		particleSystem_->y -= 0.1f * ncApplication::interval();
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
void MyEventHandler::onTouchDown(const ncTouchEvent &event)
{
	particleSystem_->x = event.x;
	particleSystem_->y = event.y;
}

void MyEventHandler::onTouchMove(const ncTouchEvent &event)
{
	particleSystem_->x = event.x;
	particleSystem_->y = event.y;

	if (event.count > 1)
	{
		emitVector_.x = (event.x2 - particleSystem_->x) * 2.5f;
		emitVector_.y = (event.y2 - particleSystem_->y) * 2.5f;
	}
}
void MyEventHandler::onAcceleration(const ncAccelerometerEvent &event)
{
	particleSystem_->x += event.y * 0.75f;
	particleSystem_->y += -event.x * 0.75f;
}
#else
void MyEventHandler::onKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
	{
		ncApplication::quit();
	}
	else if (event.sym == NCKEY_SPACE)
	{
		ncApplication::togglePause();
	}
}

void MyEventHandler::onMouseButtonPressed(const ncMouseEvent &event)
{
	if (event.isLeftButton())
	{
		particleSystem_->x = event.x;
		particleSystem_->y = event.y;
	}
}

void MyEventHandler::onMouseMoved(const ncMouseState &state)
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
