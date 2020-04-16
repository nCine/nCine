#include "apptest_meshdeform.h"
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/MeshSprite.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *TextureFile = "texture2_ETC2.ktx";
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const char *TextureFile = "texture2.png";
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const unsigned int GridSize = 9;
const unsigned int NumVertices = GridSize * GridSize;
nc::MeshSprite::Vertex vertices[GridSize * GridSize];
const unsigned int NumIndices = GridSize * 2 * (GridSize - 1) + 2 * (GridSize - 2);
unsigned short indices[NumIndices];
nc::Vector2f restVertices[GridSize * GridSize];

enum Deformations
{
	NO_DEFORMATION = 0,
	DEFORMATION_A,
	DEFORMATION_B,
	DEFORMATION_C,
	DEFORMATION_COUNT
};
unsigned int currentDeformation = DEFORMATION_A;

enum Animations
{
	NO_ANIMATION = 0,
	ANIMATION_A,
	ANIMATION_B,
	ANIMATION_C,
	ANIMATION_COUNT
};
unsigned int currentAnimation = ANIMATION_A;

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

	texture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", TextureFile)).data());

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());

	debugString_ = nctl::makeUnique<nctl::String>(128);
	debugText_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get());
	debugText_->setPosition((nc::theApplication().width() - debugText_->width()) * 0.5f,
	                        nc::theApplication().height() - debugText_->fontLineHeight() * 0.5f * 2.0f);
	debugText_->setColor(255, 255, 0, 255);
	debugText_->setAlignment(nc::TextNode::Alignment::CENTER);

	// Setting the mesh grid
	const float dist = 1.0f / static_cast<float>(GridSize - 1);
	for (unsigned int i = 0; i < GridSize; i++)
	{
		for (unsigned int j = 0; j < GridSize; j++)
		{
			nc::MeshSprite::Vertex &v = vertices[i * GridSize + j];
			v.x = dist * static_cast<float>(j) - 0.5f;
			v.y = dist * static_cast<float>(i) - 0.5f;
			v.u = dist * static_cast<float>(j);
			v.v = 1.0f - dist * static_cast<float>(i);

			restVertices[i * GridSize + j].set(v.x, v.y);
		}
	}

	unsigned short vertexIndex = GridSize;
	unsigned int arrayIndex = 0;
	for (unsigned short i = 0; i < GridSize - 1; i++)
	{
		for (unsigned short j = 0; j < GridSize; j++)
		{
			indices[arrayIndex++] = vertexIndex + j;
			if (j == 0 && i != 0) // degenerate vertex
				indices[arrayIndex++] = vertexIndex + j;
			indices[arrayIndex++] = vertexIndex + j - GridSize;
		}
		if (i != GridSize - 2) // degenerate vertex
		{
			indices[arrayIndex] = indices[arrayIndex - 1];
			arrayIndex++;
		}
		vertexIndex += GridSize;
	}

	meshSprite_ = nctl::makeUnique<nc::MeshSprite>(&rootNode, texture_.get(), nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	meshSprite_->setScale(4.0f);
	meshSprite_->setVertices(NumVertices, vertices);
	meshSprite_->setIndices(NumIndices, indices);

	pause_ = false;
	angle_ = 0.0f;

	scrollOrigin_ = nc::Vector2f::Zero;
	scrollMove_.set(nc::theApplication().width() * 0.499f, nc::theApplication().height() * 0.499f);
	joyVectorRight_ = nc::Vector2f::Zero;
}

void MyEventHandler::onFrameStart()
{
	const float interval = nc::theApplication().interval();
	if (!pause_)
		angle_ += 20.0f * interval;

	nc::Vector2f force;
	force.x = 0.5f * (scrollMove_.x / nc::theApplication().width() - 0.5f);
	force.y = 0.5f * (scrollMove_.y / nc::theApplication().height() - 0.5f);

	if (joyVectorRight_.length() > nc::IInputManager::RightStickDeadZone)
	{
		scrollMove_.set(nc::theApplication().width() * 0.499f, nc::theApplication().height() * 0.499f);
		force = joyVectorRight_ * 0.25f;
	}

	const nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
	debugString_->clear();
	debugString_->format("deformation: %u, animation: %u", currentDeformation, currentAnimation);
	debugString_->formatAppend("\nbatching: %s, culling: %s", settings.batchingEnabled ? "on" : "off", settings.cullingEnabled ? "on" : "off");
	debugText_->setString(*debugString_);

	float xAbsMax = 0.0f;
	float yAbsMax = 0.0f;
	for (unsigned int i = 0; i < GridSize; i++)
	{
		for (unsigned int j = 0; j < GridSize; j++)
		{
			nc::MeshSprite::Vertex &vertex = vertices[i * GridSize + j];
			const nc::Vector2f &restVertex = restVertices[i * GridSize + j];

			nc::Vector2f dir(restVertex - force);
			const float len = dir.sqrLength();
			dir.normalize();

			switch (currentDeformation)
			{
				case NO_DEFORMATION:
					vertex.x = restVertex.x;
					vertex.y = restVertex.y;
					break;
				case DEFORMATION_A:
					vertex.x = restVertex.x + dir.x * 0.35f;
					vertex.y = restVertex.y + dir.y * 0.35f;
					break;
				case DEFORMATION_B:
					vertex.x = restVertex.x + force.x;
					vertex.y = restVertex.y + force.y;
					break;
				case DEFORMATION_C:
					vertex.x = restVertex.x + dir.x * (1.0f - len);
					vertex.y = restVertex.y + dir.y * (1.0f - len);
					break;
			}

			switch (currentAnimation)
			{
				case NO_ANIMATION:
					break;
				case ANIMATION_A:
					vertex.x += restVertex.x * 0.5f * (sinf(angle_ * 0.15f) * 0.8f) * sinf((i + j) / float(GridSize - 1));
					vertex.y += restVertex.y * 0.5f * (cosf(angle_ * 0.15f) * 0.8f) * cosf((i + j) / float(GridSize - 1));
					break;
				case ANIMATION_B:
					vertex.x += 0.15f * sinf(0.1f * (angle_ + 1.5f * i));
					vertex.y += 0.15f * cosf(0.1f * (angle_ + 1.5f * j));
					break;
				case ANIMATION_C:
					vertex.x += 0.5f * sinf(angle_ * 0.1f);
					vertex.y += 0.5f * cosf(angle_ * 0.1f);
					break;
			}

			if (fabsf(vertex.x) > xAbsMax)
				xAbsMax = fabsf(vertex.x);
			if (fabsf(vertex.y) > yAbsMax)
				yAbsMax = fabsf(vertex.y);
		}
	}

	for (unsigned int i = 0; i < GridSize * GridSize; i++)
	{
		vertices[i].x = (vertices[i].x / xAbsMax) * 0.5f;
		vertices[i].y = (vertices[i].y / yAbsMax) * 0.5f;
	}
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	scrollOrigin_.x = event.pointers[0].x;
	scrollOrigin_.y = event.pointers[0].y;
	scrollMove_ = scrollOrigin_;

	checkClick(event.pointers[0].x, event.pointers[0].y);
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	scrollMove_.x = event.pointers[0].x;
	scrollMove_.y = event.pointers[0].y;
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();
	nc::IDebugOverlay::DisplaySettings &overlaySettings = nc::theApplication().debugOverlaySettings();

	if (event.sym == nc::KeySym::RIGHT || event.sym == nc::KeySym::D)
		currentDeformation = (currentDeformation + 1) % DEFORMATION_COUNT;
	else if (event.sym == nc::KeySym::LEFT || event.sym == nc::KeySym::A)
		currentAnimation = (currentAnimation + 1) % ANIMATION_COUNT;
	else if (event.sym == nc::KeySym::B)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.sym == nc::KeySym::C)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
	else if (event.sym == nc::KeySym::H)
	{
		overlaySettings.showProfilerGraphs = !overlaySettings.showProfilerGraphs;
		overlaySettings.showInfoText = !overlaySettings.showInfoText;
	}
	else if (event.sym == nc::KeySym::BACKQUOTE)
		overlaySettings.showInterface = !overlaySettings.showInterface;
	else if (event.sym == nc::KeySym::P)
		pause_ = !pause_;
	else if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
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
		scrollOrigin_.x = static_cast<float>(event.x);
		scrollOrigin_.y = static_cast<float>(event.y);
		scrollMove_ = scrollOrigin_;

		checkClick(static_cast<float>(event.x), static_cast<float>(event.y));
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (state.isLeftButtonDown())
	{
		scrollMove_.x = static_cast<float>(state.x);
		scrollMove_.y = static_cast<float>(state.y);
	}
}

void MyEventHandler::onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event)
{
	if (event.axisName == nc::AxisName::RX)
		joyVectorRight_.x = event.value;
	else if (event.axisName == nc::AxisName::RY)
		joyVectorRight_.y = -event.value;
}

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();
	nc::IDebugOverlay::DisplaySettings &overlaySettings = nc::theApplication().debugOverlaySettings();

	if (event.buttonName == nc::ButtonName::RBUMPER)
		currentAnimation = (currentAnimation + 1) % ANIMATION_COUNT;
	else if (event.buttonName == nc::ButtonName::LBUMPER)
		currentDeformation = (currentDeformation + 1) % DEFORMATION_COUNT;
	if (event.buttonName == nc::ButtonName::A)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.buttonName == nc::ButtonName::Y)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
	else if (event.buttonName == nc::ButtonName::BACK)
	{
		overlaySettings.showProfilerGraphs = !overlaySettings.showProfilerGraphs;
		overlaySettings.showInfoText = !overlaySettings.showInfoText;
	}
	else if (event.buttonName == nc::ButtonName::START)
		pause_ = !pause_;
	else if (event.buttonName == nc::ButtonName::GUIDE)
		nc::theApplication().quit();
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	joyVectorRight_ = nc::Vector2f::Zero;
}

void MyEventHandler::checkClick(float x, float y)
{
	const nc::Rectf debugTextRect = nc::Rectf::fromCenterAndSize(debugText_->absPosition(), debugText_->absSize());

	if (debugTextRect.contains(x, y))
	{
		if (y <= debugTextRect.y + debugTextRect.h * 0.5f)
		{
			nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
			const float xPos = x - debugTextRect.x;
			if (xPos >= debugTextRect.w * 0.09f && xPos <= debugTextRect.w * 0.5f)
				settings.batchingEnabled = !settings.batchingEnabled;
			else if (xPos >= debugTextRect.w * 0.5f && xPos <= debugTextRect.w * 0.91f)
				settings.cullingEnabled = !settings.cullingEnabled;
		}
	}
	else if (x >= nc::theApplication().width() * 0.8f)
		currentAnimation = (currentAnimation + 1) % ANIMATION_COUNT;
	else if (x <= nc::theApplication().width() * 0.2f)
		currentDeformation = (currentDeformation + 1) % DEFORMATION_COUNT;
}
