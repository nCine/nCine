#include "common_constants.h"
#include "apptest_camera.h"
#include "Application.h"
#include "Texture.h"
#include "Sprite.h"
#include "TextNode.h"
#include "Timer.h"
#include "IFile.h" // for dataPath()
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
	const char *Texture1File = "texture1.webp";
	const char *Texture2File = "texture2.webp";
	const char *Texture3File = "texture3.webp";
	const char *Texture4File = "texture4.webp";
	const char *FontTextureFile = "DroidSans32_256.webp";
#else
	const char *Texture1File = "texture1.png";
	const char *Texture2File = "texture2.png";
	const char *Texture3File = "texture3.png";
	const char *Texture4File = "texture4.png";
	const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const float MoveSpeed = 500.0f;
const float RotateSpeed = 30.0f;
const float ScaleSpeed = 0.3f;

const float MaxCameraScale = 3.0f;
const float MinCameraScale = 0.5f;
const float DoubleClickDelay = 0.3f;

const float ViewHalfWidth = 1500.0f;
const float ViewHalfHeight = 1000.0f;

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

	textures_[0] = new nc::Texture((nc::IFile::dataPath() + "textures/" + Texture1File).data());
	textures_[1] = new nc::Texture((nc::IFile::dataPath() + "textures/" + Texture2File).data());
	textures_[2] = new nc::Texture((nc::IFile::dataPath() + "textures/" + Texture3File).data());
	textures_[3] = new nc::Texture((nc::IFile::dataPath() + "textures/" + Texture4File).data());

	font_ = new nc::Font((nc::IFile::dataPath() + "fonts/" + FontTextureFile).data(),
	                     (nc::IFile::dataPath() + "fonts/" + FontFntFile).data());

	cameraNode_ = new nc::SceneNode(&rootNode);

	debugString_ = new nc::String(64);
	debugtext_ = new nc::TextNode(&rootNode, font_);
	debugtext_->setPosition((nc::theApplication().width() - debugtext_->width()) * 0.5f,
	                        nc::theApplication().height() - debugtext_->fontLineHeight() * 0.5f);
	debugtext_->setColor(255, 255, 0, 255);

	for (unsigned int i = 0; i < NumTexts; i++)
	{
		texts_[i] = new nc::TextNode(cameraNode_, font_);
		texts_[i]->setScale(1.5f);
		texts_[i]->setColor(0, 0, 255, 255);
	}

	nc::String textString(16);
	texts_[0]->setPosition(0.0f, 0.0f);
	texts_[0]->setString("(0, 0)");

	textString.format("(%.0f, %.0f)", ViewHalfWidth, ViewHalfHeight);
	texts_[1]->setString(textString);
	texts_[1]->setPosition(ViewHalfWidth - texts_[1]->width() * 0.5f, ViewHalfHeight - texts_[1]->height() * 0.5f);

	textString.format("(%.0f, %.0f)", -ViewHalfWidth, ViewHalfHeight);
	texts_[2]->setString(textString);
	texts_[2]->setPosition(-ViewHalfWidth + texts_[2]->width() * 0.5f, ViewHalfHeight - texts_[2]->height() * 0.5f);

	textString.format("(%.0f, %.0f)", ViewHalfWidth, -ViewHalfHeight);
	texts_[3]->setString(textString);
	texts_[3]->setPosition(ViewHalfWidth - texts_[3]->width() * 0.5f, -ViewHalfHeight + texts_[3]->height());

	textString.format("(%.0f, %.0f)", -ViewHalfWidth, -ViewHalfHeight);
	texts_[4]->setString(textString);
	texts_[4]->setPosition(-ViewHalfWidth + texts_[4]->width() * 0.5f, -ViewHalfHeight + texts_[4]->height());

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		float randomX = nc::randBetween(-ViewHalfWidth, ViewHalfWidth);
		float randomY = nc::randBetween(-ViewHalfHeight, ViewHalfHeight);
		sprites_[i] = new nc::Sprite(cameraNode_, textures_[i % NumTextures], randomX, randomY);
		sprites_[i]->setScale(0.5f);
		spritePos_[i].set(randomX, randomY);
	}

	angle_ = 0.0f;
	pause_ = false;
	resetCamera();

	scrollOrigin_ = nc::Vector2f::Zero;
	scrollMove_ = nc::Vector2f::Zero;
	scrollOrigin2_ = nc::Vector2f::Zero;
	scrollMove2_ = nc::Vector2f::Zero;
	lastClick_ = 0.0f;
	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}

void MyEventHandler::onFrameStart()
{
	float interval = nc::theApplication().interval();
	if (!pause_) { angle_ += 2.0f * interval; }

	const nc::KeyboardState &keyState = nc::theApplication().inputManager().keyboardState();

	if (keyState.isKeyDown(nc::KEY_D))
	{
		camPos_.x -= MoveSpeed * interval;
	}
	else if (keyState.isKeyDown(nc::KEY_A))
	{
		camPos_.x += MoveSpeed * interval;
	}
	if (keyState.isKeyDown(nc::KEY_W))
	{
		camPos_.y -= MoveSpeed * interval;
	}
	else if (keyState.isKeyDown(nc::KEY_S))
	{
		camPos_.y += MoveSpeed * interval;
	}

	if (keyState.isKeyDown(nc::KEY_RIGHT))
	{
		camRot_ += RotateSpeed * interval;
	}
	else if (keyState.isKeyDown(nc::KEY_LEFT))
	{
		camRot_ -= RotateSpeed * interval;
	}

	if (keyState.isKeyDown(nc::KEY_UP))
	{
		camScale_ += ScaleSpeed * interval;
	}
	else if (keyState.isKeyDown(nc::KEY_DOWN))
	{
		camScale_ -= ScaleSpeed * interval;
	}

	if (joyVectorLeft_.length() > nc::IInputManager::LeftStickDeadZone)
	{
		camPos_.x -= joyVectorLeft_.x * MoveSpeed * interval;
		camPos_.y -= joyVectorLeft_.y * MoveSpeed * interval;
	}
	if (joyVectorRight_.length() > nc::IInputManager::LeftStickDeadZone)
	{
		camRot_ += joyVectorRight_.x * RotateSpeed * interval;
		camScale_ += joyVectorRight_.y * ScaleSpeed * interval;
	}

	nc::Vector2f scrollDiff = scrollMove_ - scrollOrigin_;
	if (scrollDiff.sqrLength() > 2.0f)
	{
		camPos_.x += scrollDiff.x;
		camPos_.y += scrollDiff.y;
		scrollOrigin_ = scrollMove_;
	}
	nc::Vector2f scrollDiff2 = scrollMove2_ - scrollOrigin2_;
	if (scrollDiff2.sqrLength() > 2.0f)
	{
		camRot_ += scrollDiff2.x * 0.1f;
		camScale_ += scrollDiff2.y * 0.001f;
		scrollOrigin2_ = scrollMove2_;
	}

	if (camScale_ > MaxCameraScale) { camScale_ = MaxCameraScale; }
	else if (camScale_ < MinCameraScale) { camScale_ = MinCameraScale; }

	float scaledWidth = ViewHalfWidth * 2.0f * camScale_;
	float scaledHeight = ViewHalfHeight * 2.0f * camScale_;

	float rotatedWidth = scaledWidth;
	float rotatedHeight = scaledHeight;
	if (camRot_ > 0.01f || camRot_ < -0.01f)
	{
		camRot_ = fmod(camRot_, 360.0f);
		float sinRot = sinf(camRot_ * nc::fDegToRad);
		float cosRot = cosf(camRot_ * nc::fDegToRad);
		rotatedWidth = fabsf(scaledHeight * sinRot) + fabsf(scaledWidth * cosRot);
		rotatedHeight = fabsf(scaledWidth * sinRot) + fabsf(scaledHeight * cosRot);
	}

	float maxX = rotatedWidth * 0.5f;
	float minX = (-rotatedWidth * 0.5f) + nc::theApplication().width();;
	float maxY = rotatedHeight * 0.5f;
	float minY = (-rotatedHeight * 0.5f) + nc::theApplication().height();;

	if (camPos_.x > maxX) {camPos_.x = maxX; }
	if (camPos_.x < minX) {camPos_.x = minX; }
	if (camPos_.y > maxY) {camPos_.y = maxY; }
	if (camPos_.y < minY) {camPos_.y = minY; }

	debugString_->clear();
	debugString_->format("x: %.2f, y: %.2f, scale: %.2f, angle: %.2f", -camPos_.x, -camPos_.y, camScale_, camRot_);
	debugtext_->setString(*debugString_);

	cameraNode_->setPosition(camPos_);
	cameraNode_->setRotation(camRot_);
	cameraNode_->setScale(camScale_);

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		float t = i / static_cast<float>(NumSprites - 1);
		float scaleX = 50.0f * (2.0f * t - 1.0f);
		float scaleY = 50.0f * (-2.0f * t + 1.0f);
		float moveX = scaleX * sinf(angle_) * cosf(angle_ * 0.5f * t);
		float moveY = scaleY * sinf(angle_ * 0.5f * t) * cosf(angle_);
		sprites_[i]->setPosition(spritePos_[i].x + moveX, spritePos_[i].y + moveY);
	}
}

void MyEventHandler::onShutdown()
{
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		delete sprites_[i];
	}

	for (unsigned int i = 0; i < NumTexts; i++)
	{
		delete texts_[i];
	}

	delete debugtext_;
	delete debugString_;
	delete cameraNode_; // and all its children
	delete font_;

	for (unsigned int i = 0; i < NumTextures; i++)
	{
		delete textures_[i];
	}
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	scrollOrigin_.x = event.pointers[0].x;
	scrollOrigin_.y = event.pointers[0].y;
	scrollMove_ = scrollOrigin_;
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	scrollMove_.x = event.pointers[0].x;
	scrollMove_.y = event.pointers[0].y;

	if (event.count > 1)
	{
		scrollMove2_.x = event.pointers[1].x;
		scrollMove2_.y = event.pointers[1].y;
	}
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	if (nc::Timer::now() - lastClick_ < DoubleClickDelay)
	{
		resetCamera();
	}
	lastClick_ = nc::Timer::now();
}

void MyEventHandler::onPointerDown(const nc::TouchEvent &event)
{
	if (event.count == 2)
	{
		scrollOrigin2_.x = event.pointers[1].x;
		scrollOrigin2_.y = event.pointers[1].y;
		scrollMove2_ = scrollOrigin2_;
	}
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KEY_P)
	{
		pause_ = !pause_;
	}
	else if (event.sym == nc::KEY_R)
	{
		resetCamera();
	}
	else if (event.sym == nc::KEY_ESCAPE || event.sym == nc::KEY_Q)
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
		scrollOrigin_.x = static_cast<float>(event.x);
		scrollOrigin_.y = static_cast<float>(event.y);
		scrollMove_ = scrollOrigin_;
	}
	else if (event.isRightButton())
	{
		scrollOrigin2_.x = static_cast<float>(event.x);
		scrollOrigin2_.y = static_cast<float>(event.y);
		scrollMove2_ = scrollOrigin2_;
	}
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
	{
		if (nc::Timer::now() - lastClick_ < DoubleClickDelay)
		{
			resetCamera();
		}
		lastClick_ = nc::Timer::now();
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (state.isLeftButtonDown())
	{
		scrollMove_.x = static_cast<float>(state.x);
		scrollMove_.y = static_cast<float>(state.y);
	}
	else if (state.isRightButtonDown())
	{
		scrollMove2_.x = static_cast<float>(state.x);
		scrollMove2_.y = static_cast<float>(state.y);
	}
}

void MyEventHandler::onScrollInput(const nc::ScrollEvent &event)
{
	camRot_ += 10.0f * event.x;
	camScale_ += 0.1f * event.y;
}

void MyEventHandler::onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event)
{
	if (event.axisName == nc::AXIS_LX)	{ joyVectorLeft_.x = event.value; }
	else if (event.axisName == nc::AXIS_LY) { joyVectorLeft_.y = -event.value; }

	if (event.axisName == nc::AXIS_RX)	{ joyVectorRight_.x = event.value; }
	else if (event.axisName == nc::AXIS_RY) { joyVectorRight_.y = -event.value; }
}

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	if (event.buttonName == nc::BUTTON_B)
	{
		resetCamera();
	}
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}

void MyEventHandler::resetCamera()
{
	camPos_.x = nc::theApplication().width() * 0.5f;
	camPos_.y = nc::theApplication().height() * 0.5f;
	camRot_ = 0.0f;
	camScale_ = 1.0f;
}
