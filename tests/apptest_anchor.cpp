#include <ncine/imgui.h>

#include "apptest_anchor.h"
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/MeshSprite.h>
#include <ncine/TextNode.h>
#include <ncine/ParticleSystem.h>
#include <ncine/ParticleInitializer.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *Texture1File = "texture1_ETC2.ktx";
const char *Texture2File = "texture2_ETC2.ktx";
const char *Texture3File = "texture3_ETC2.ktx";
const char *Texture4File = "texture4_ETC2.ktx";
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const char *Texture1File = "texture1.png";
const char *Texture2File = "texture2.png";
const char *Texture3File = "texture3.png";
const char *Texture4File = "texture4.png";
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const int NumTexelPoints[MyEventHandler::NumTextures] = { 3, 4, 5, 4 };
const nc::Vector2f TexelPoints[] = {
	// clang-format off
	{4.0f, 2.0f}, {124.0f, 2.0f}, {64.0f, 122.0f},
	{13.0f, 13.0f}, {115.0f, 13.0f}, {13.0f, 115.0f}, {115.0f, 115.0f},
	{3.0f, 79.0f}, {26.0f, 2.0f}, {64.0f, 125.0f}, {102.0f, 2.0f}, {125.0f, 79.0f},
	{20.0f, 20.0f}, {107.0f, 20.0f}, {20.0f, 107.0f}, {107.0f, 107.0f},
	// clang-format on
};

const char *TypeLables[] = { "Sprites", "Mesh Sprites", "Text Nodes", "Particle Systems" };
const char *AnchorPoints[] = { "Center", "Bottom Left", "Top Left", "Bottom Right", "Top Right" };
const char *BlendingPresets[] = { "Disabled", "Alpha", "Pre-multiplied Alpha", "Additive", "Multiply" };

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

MyEventHandler::MyEventHandler()
    : background_(nc::Colorf::Black),
      currentType_(0), anchorPoint_(0.5f, 0.5f),
      position_(0.0f, 0.0f), angle_(0.0f),
      scale_(1.0f, 1.0f), lockScale_(true),
      flippedX_(false), flippedY_(false),
      color_(nc::Colorf::White), blendingEnabled_(true),
      blendingPreset_(nc::DrawableNode::BlendingPreset::ALPHA)
{
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());

	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture1File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture2File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture3File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture4File)).data()));

	const float width = nc::theApplication().width();
	const float height = nc::theApplication().height();
	int texelStartIndex = 0;
	nctl::String textNodeString(32);
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		nc::Texture *texture = textures_[i % NumTextures].get();
		const nc::Vector2f position(width / (NumSprites + 3) * (i + 2), height * 0.5f);

		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(&rootNode, texture, position));
		sprites_.back()->setEnabled(false);

		meshSprites_.pushBack(nctl::makeUnique<nc::MeshSprite>(&rootNode, texture, position));
		meshSprites_.back()->createVerticesFromTexels(NumTexelPoints[i % NumTextures], &TexelPoints[texelStartIndex]);
		texelStartIndex += NumTexelPoints[i % NumTextures];
		if (texelStartIndex >= static_cast<int>(sizeof(TexelPoints) / sizeof(*TexelPoints)))
			texelStartIndex = 0;

		textNodes_.pushBack(nctl::makeUnique<nc::TextNode>(&rootNode, font_.get()));
		textNodeString.format("This is\nTextNode #%u", i);
		textNodes_.back()->setString(textNodeString);
		textNodes_.back()->setAlignment(static_cast<nc::TextNode::Alignment>(i % 3));

		particleSystems_.pushBack(nctl::makeUnique<nc::ParticleSystem>(&rootNode, 16, texture, texture->rect()));
		particleSystems_.back()->setPosition(position);

		nctl::UniquePtr<nc::ColorAffector> colAffector = nctl::makeUnique<nc::ColorAffector>();
		colAffector->addColorStep(0.0f, nc::Colorf(1.0f, 1.0f, 1.0f, 1.0f));
		colAffector->addColorStep(1.0f, nc::Colorf(1.0f, 1.0f, 1.0f, 0.0f));
		particleSystems_.back()->addAffector(nctl::move(colAffector));
		nctl::UniquePtr<nc::SizeAffector> sizeAffector = nctl::makeUnique<nc::SizeAffector>(1.0f);
		sizeAffectors_.pushBack(sizeAffector.get());
		particleSystems_.back()->addAffector(nctl::move(sizeAffector));
		nctl::UniquePtr<nc::RotationAffector> rotationAffector = nctl::makeUnique<nc::RotationAffector>();
		rotationAffector->addRotationStep(0.0f, 0.0f);
		rotationAffector->addRotationStep(1.0f, 0.0f);
		rotationAffectors_.pushBack(rotationAffector.get());
		particleSystems_.back()->addAffector(nctl::move(rotationAffector));
	}
}

void MyEventHandler::onFrameStart()
{
	nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();

	const float width = nc::theApplication().width();
	const float height = nc::theApplication().height();

	ImGui::Begin("apptest_anchor");
	if (ImGui::Combo("Type", &currentType_, TypeLables, IM_ARRAYSIZE(TypeLables)))
	{
		if (currentType_ == Type::PARTICLE_SYSTEM)
			lastEmissionTime_ = nc::TimeStamp::now();
	}
	ImGui::SliderFloat2("Anchor", anchorPoint_.data(), 0.0f, 1.0f);
	static int currentAnchorSelection = 0;
	if (ImGui::Combo("Presets", &currentAnchorSelection, AnchorPoints, IM_ARRAYSIZE(AnchorPoints)))
	{
		switch (currentAnchorSelection)
		{
			case 0:
				anchorPoint_ = nc::DrawableNode::AnchorCenter;
				break;
			case 1:
				anchorPoint_ = nc::DrawableNode::AnchorBottomLeft;
				break;
			case 2:
				anchorPoint_ = nc::DrawableNode::AnchorTopLeft;
				break;
			case 3:
				anchorPoint_ = nc::DrawableNode::AnchorBottomRight;
				break;
			case 4:
				anchorPoint_ = nc::DrawableNode::AnchorTopRight;
				break;
		}
	}
	ImGui::SliderFloat("Position X", &position_.x, -width * 0.75f, width * 0.75f);
	ImGui::SameLine();
	if (ImGui::Button("Reset##Position"))
		position_.set(0.0f, 0.0f);
	ImGui::SliderFloat("Position Y", &position_.y, -height * 0.75f, height * 0.75f);
	ImGui::SliderFloat("Rotation", &angle_, 0.0f, 360.0f);
	ImGui::SameLine();
	if (ImGui::Button("Reset##Rotation"))
		angle_ = 0.0f;
	if (lockScale_)
	{
		ImGui::SliderFloat("Scale", &scale_.x, 0.5f, 2.0f);
		scale_.y = scale_.x;
		ImGui::SameLine();
		ImGui::Checkbox("Lock", &lockScale_);
	}
	else
	{
		ImGui::SliderFloat("Scale X", &scale_.x, 0.5f, 2.0f);
		ImGui::SameLine();
		ImGui::Checkbox("Lock", &lockScale_);
		ImGui::SliderFloat("Scale Y", &scale_.y, 0.5f, 2.0f);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset##Scale"))
		scale_.set(1.0f, 1.0f);
	if (currentType_ != Type::TEXT_NODE)
	{
		ImGui::Checkbox("Flipped X", &flippedX_);
		ImGui::SameLine();
		ImGui::Checkbox("Flipped Y", &flippedY_);
		ImGui::SameLine();
		if (ImGui::Button("Reset##Flip"))
		{
			flippedX_ = false;
			flippedY_ = false;
		}
	}
	ImGui::ColorEdit4("Color", color_.data());
	static int currentBlendingSelection = 1;
	if (ImGui::Combo("Blending", &currentBlendingSelection, BlendingPresets, IM_ARRAYSIZE(BlendingPresets)))
		blendingPreset_ = static_cast<nc::DrawableNode::BlendingPreset>(currentBlendingSelection);
	if (currentType_ != Type::PARTICLE_SYSTEM)
	{
		ImGui::SameLine();
		ImGui::Checkbox("Enabled", &blendingEnabled_);
	}

	ImGui::ColorEdit4("Background Color", background_.data(), ImGuiColorEditFlags_NoAlpha);
	nc::theApplication().gfxDevice().setClearColor(background_);
	ImGui::Checkbox("Culling", &settings.cullingEnabled);
	ImGui::SameLine();
	ImGui::Checkbox("Batching", &settings.batchingEnabled);
	ImGui::End();

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		if (currentType_ == Type::SPRITE)
		{
			sprites_[i]->setAnchorPoint(anchorPoint_);
			sprites_[i]->x = position_.x + width / (NumSprites + 3) * (i + 2);
			sprites_[i]->y = position_.y + (height * 0.5f);
			sprites_[i]->setRotation(angle_);
			sprites_[i]->setScale(scale_);
			sprites_[i]->setFlippedX(flippedX_);
			sprites_[i]->setFlippedY(flippedY_);
			sprites_[i]->setColor(color_);
			sprites_[i]->setBlendingEnabled(blendingEnabled_);
			sprites_[i]->setBlendingPreset(blendingPreset_);
		}
		else if (currentType_ == Type::MESH_SPRITE)
		{
			meshSprites_[i]->setAnchorPoint(anchorPoint_);
			meshSprites_[i]->x = position_.x + width / (NumSprites + 3) * (i + 2);
			meshSprites_[i]->y = position_.y + (height * 0.5f);
			meshSprites_[i]->setRotation(angle_);
			meshSprites_[i]->setScale(scale_);
			meshSprites_[i]->setFlippedX(flippedX_);
			meshSprites_[i]->setFlippedY(flippedY_);
			meshSprites_[i]->setColor(color_);
			meshSprites_[i]->setBlendingEnabled(blendingEnabled_);
			meshSprites_[i]->setBlendingPreset(blendingPreset_);
		}
		else if (currentType_ == Type::TEXT_NODE)
		{
			textNodes_[i]->setAnchorPoint(anchorPoint_);
			textNodes_[i]->x = position_.x + width / (NumSprites + 3) * (i + 2);
			textNodes_[i]->y = position_.y + (height * 0.5f);
			textNodes_[i]->setRotation(angle_);
			textNodes_[i]->setScale(scale_);
			textNodes_[i]->setColor(color_);
			textNodes_[i]->setBlendingEnabled(blendingEnabled_);
			textNodes_[i]->setBlendingPreset(blendingPreset_);
		}
		else if (currentType_ == Type::PARTICLE_SYSTEM)
		{
			particleSystems_[i]->setAnchorPoint(anchorPoint_);
			particleSystems_[i]->x = position_.x + width / (NumSprites + 3) * (i + 2);
			particleSystems_[i]->y = position_.y + (height * 0.5f);
			rotationAffectors_[i]->steps()[1].angle = angle_;
			sizeAffectors_[i]->setBaseScale(scale_);
			particleSystems_[i]->setFlippedX(flippedX_);
			particleSystems_[i]->setFlippedY(flippedY_);
			particleSystems_[i]->setColor(color_);
			particleSystems_[i]->setBlendingPreset(blendingPreset_);
		}

		sprites_[i]->setEnabled(currentType_ == Type::SPRITE);
		meshSprites_[i]->setEnabled(currentType_ == Type::MESH_SPRITE);
		textNodes_[i]->setEnabled(currentType_ == Type::TEXT_NODE);
		particleSystems_[i]->setEnabled(currentType_ == Type::PARTICLE_SYSTEM);
	}

	if (currentType_ == Type::PARTICLE_SYSTEM && lastEmissionTime_.secondsSince() > 0.1f)
	{
		nc::ParticleInitializer init;
		init.setAmount(1);
		init.setLife(0.5f, 0.75f);
		init.setPositionAndRadius(nc::Vector2f::Zero, 10.0f);
		init.setVelocityAndScale(nc::Vector2f(0.0f, 250.0f), 0.8f, 1.0f);

		lastEmissionTime_ = nc::TimeStamp::now();
		for (unsigned int i = 0; i < NumSprites; i++)
			particleSystems_[i]->emitParticles(init);
	}
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
}
