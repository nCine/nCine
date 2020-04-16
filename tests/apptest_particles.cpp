#include "apptest_particles.h"
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/ParticleSystem.h>
#include <ncine/ParticleInitializer.h>
#include "apptest_datapath.h"

#ifdef __ANDROID__
	#include "AndroidApplication.h"
#endif

namespace {

#ifdef __ANDROID__
const char *TextureFile = "smoke_256_ETC2.ktx";
#else
const char *TextureFile = "smoke_256.png";
#endif

const float KeySpeed = 200.0f;
const float JoySpeed = 200.0f;

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

#ifdef __ANDROID__
	nc::AndroidApplication &application = static_cast<nc::AndroidApplication &>(nc::theApplication());
	application.enableAccelerometer(true);
#endif

	texture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", TextureFile)).data());
	particleSystem_ = nctl::makeUnique<nc::ParticleSystem>(&rootNode, unsigned(NumParticles), texture_.get(), texture_->rect());
	particleSystem_->setPosition(nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.33f);

	nctl::UniquePtr<nc::ColorAffector> colAffector = nctl::makeUnique<nc::ColorAffector>();
	colAffector->addColorStep(0.0f, nc::Colorf(0.0f, 0.0f, 1.0f, 0.9f));
	colAffector->addColorStep(0.3f, nc::Colorf(0.86f, 0.7f, 0.0f, 0.65f));
	colAffector->addColorStep(0.35f, nc::Colorf(0.86f, 0.59f, 0.0f, 0.8f));
	colAffector->addColorStep(1.0f, nc::Colorf(0.86f, 0.39f, 0.0f, 0.75f));
	particleSystem_->addAffector(nctl::move(colAffector));
	nctl::UniquePtr<nc::SizeAffector> sizeAffector = nctl::makeUnique<nc::SizeAffector>(0.45f);
	sizeAffector->addSizeStep(0.0f, 0.4f);
	sizeAffector->addSizeStep(0.3f, 1.7f);
	sizeAffector->addSizeStep(1.0f, 0.01f);
	particleSystem_->addAffector(nctl::move(sizeAffector));
	emitVector_.set(0.0f, 350.0f);

	lastEmissionTime_ = nc::TimeStamp::now();

	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}

void MyEventHandler::onFrameStart()
{
	const float interval = nc::theApplication().interval();

	if (lastEmissionTime_.secondsSince() > 0.085f)
	{
		nc::ParticleInitializer init;
		init.setAmount(3);
		init.setLife(0.85f, 1.0f);
		init.setPositionAndRadius(nc::Vector2f::Zero, 10.0f);
		init.setVelocityAndScale(emitVector_, 0.8f, 1.0f);

		lastEmissionTime_ = nc::TimeStamp::now();
		particleSystem_->emitParticles(init);
	}

	particleSystem_->move(joyVectorLeft_ * JoySpeed * interval);
	emitVector_ += joyVectorRight_ * interval;

	const nc::KeyboardState &keyState = nc::theApplication().inputManager().keyboardState();

	if (keyState.isKeyDown(nc::KeySym::D))
		particleSystem_->x += KeySpeed * interval;
	else if (keyState.isKeyDown(nc::KeySym::A))
		particleSystem_->x -= KeySpeed * interval;
	if (keyState.isKeyDown(nc::KeySym::W))
		particleSystem_->y += KeySpeed * interval;
	else if (keyState.isKeyDown(nc::KeySym::S))
		particleSystem_->y -= KeySpeed * interval;

	if (keyState.isKeyDown(nc::KeySym::RIGHT))
		emitVector_.x += KeySpeed * interval;
	else if (keyState.isKeyDown(nc::KeySym::LEFT))
		emitVector_.x -= KeySpeed * interval;
	if (keyState.isKeyDown(nc::KeySym::UP))
		emitVector_.y += KeySpeed * interval;
	else if (keyState.isKeyDown(nc::KeySym::DOWN))
		emitVector_.y -= KeySpeed * interval;
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
#endif

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

void MyEventHandler::onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event)
{
	if (event.axisName == nc::AxisName::LX)
		joyVectorLeft_.x = event.value;
	else if (event.axisName == nc::AxisName::LY)
		joyVectorLeft_.y = -event.value;

	if (event.axisName == nc::AxisName::RX)
		joyVectorRight_.x = event.value;
	else if (event.axisName == nc::AxisName::RY)
		joyVectorRight_.y = -event.value;

	nc::theApplication().inputManager().deadZoneNormalize(joyVectorLeft_, nc::IInputManager::LeftStickDeadZone);
	nc::theApplication().inputManager().deadZoneNormalize(joyVectorRight_, nc::IInputManager::RightStickDeadZone);
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}
