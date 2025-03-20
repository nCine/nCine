#include <ncine/imgui.h>

#include "apptest_anchor.h"
#include <nctl/StaticString.h>
#include <ncine/Application.h>
#include <ncine/Viewport.h>
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

const char *TypeLabels[] = { "Sprites", "Mesh Sprites", "Text Nodes", "Particle Systems" };
const char *AnchorPoints[] = { "Center", "Bottom Left", "Top Left", "Bottom Right", "Top Right" };
const char *BlendingPresets[] = { "Disabled", "Alpha", "Pre-multiplied Alpha", "Additive", "Multiply" };

nctl::StaticString<128> auxString;
uint16_t DefaultImGuiLayer = 0;
bool showImGui = true;

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
      blendingPreset_(nc::DrawableNode::BlendingPreset::ALPHA),
      overlapping_(false)
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

	DefaultImGuiLayer = nc::theApplication().guiSettings().imguiLayer;
}

void MyEventHandler::onFrameStart()
{
	nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();

	const float width = nc::theApplication().width();
	const float height = nc::theApplication().height();

	if (showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(470.0f, 470.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20.0f, 100.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("apptest_anchor", &showImGui))
		{
			if (ImGui::Combo("Type", &currentType_, TypeLabels, IM_ARRAYSIZE(TypeLabels)))
			{
				if (currentType_ == Type::PARTICLE_SYSTEM)
					lastEmissionTime_ = nc::TimeStamp::now();
			}

			ImGui::Separator();
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

			ImGui::Separator();
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
			nc::theApplication().screenViewport().setClearColor(background_);

			ImGui::Separator();
			unsigned int childOrderIndices[NumSprites];
			unsigned int visitOrderIndices[NumSprites];
			int layers[NumSprites];
			auxString.clear();
			for (unsigned int i = 0; i < NumSprites; i++)
			{
				nc::SceneNode *node = castToNode(i);
				nc::DrawableNode *drawableNode = static_cast<nc::DrawableNode *>(node);
				auxString.formatAppend("%u", node->id());
				if (i < NumSprites - 1)
					auxString.append(", ");

				childOrderIndices[i] = node->childOrderIndex();
				visitOrderIndices[i] = node->visitOrderIndex();
				layers[i] = drawableNode->layer();
			}
			ImGui::Text("Id order: %s", auxString.data());

			auxString.clear();
			for (unsigned int i = 0; i < NumSprites; i++)
			{
				auxString.formatAppend("%u", childOrderIndices[i]);
				if (i < NumSprites - 1)
					auxString.append(", ");
			}
			ImGui::Text("Child order: %s", auxString.data());

			auxString.clear();
			for (unsigned int i = 0; i < NumSprites; i++)
			{
				auxString.formatAppend("%u", visitOrderIndices[i]);
				if (i < NumSprites - 1)
					auxString.append(", ");
			}
			ImGui::Text("Visit order: %s", auxString.data());

			nc::SceneNode &rootNode = nc::theApplication().rootNode();
			ImGui::SameLine();
			bool withVisitOrder = (rootNode.visitOrderState() != nc::SceneNode::VisitOrderState::DISABLED);
			ImGui::Checkbox("Use visit order", &withVisitOrder);
			if (withVisitOrder)
				rootNode.setVisitOrderState(nc::SceneNode::VisitOrderState::ENABLED);
			else
				rootNode.setVisitOrderState(nc::SceneNode::VisitOrderState::DISABLED);

			ImGui::TextUnformatted("Layer order:");
			ImGui::PushItemWidth(95.0f);
			for (unsigned int i = 0; i < NumSprites; i++)
			{
				auxString.format("##LayerOrder%d", i);
				ImGui::InputInt(auxString.data(), &layers[i]);
				if (layers[i] < 0)
					layers[i] = 0;
				else if (layers[i] > 0xFFFF)
					layers[i] = 0xFFFF;
				if (i < NumSprites - 1)
					ImGui::SameLine();
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button("Reset##LayerOrder"))
			{
				for (unsigned int i = 0; i < NumSprites; i++)
					layers[i] = 0;
			}

			for (unsigned int i = 0; i < NumSprites; i++)
			{
				nc::SceneNode *node = castToNode(i);
				nc::DrawableNode *drawableNode = static_cast<nc::DrawableNode *>(node);
				drawableNode->setLayer(layers[i]);

				if (currentType_ == Type::PARTICLE_SYSTEM)
					particleSystems_[i]->setLayer(static_cast<uint16_t>(layers[i]));
			}

			ImGui::TextUnformatted("Swap:");
			for (unsigned int i = 0; i < NumSprites - 1; i++)
			{
				nc::SceneNode *firstNode = castToNode(i);
				nc::SceneNode *secondNode = castToNode(i + 1);
				if (firstNode->parent() != nullptr && firstNode->parent() == secondNode->parent())
				{
					const unsigned int firstId = firstNode->id();
					const unsigned int secondId = secondNode->id();

					ImGui::SameLine();
					auxString.format("%d<->%d", firstId, secondId);
					if (ImGui::Button(auxString.data()))
					{
						const unsigned int firstIndex = firstNode->childOrderIndex();
						const unsigned int secondIndex = secondNode->childOrderIndex();
						firstNode->parent()->swapChildrenNodes(firstIndex, secondIndex);
						if (currentType_ == Type::SPRITE)
							nctl::swap(sprites_[i], sprites_[i + 1]);
						else if (currentType_ == Type::MESH_SPRITE)
							nctl::swap(meshSprites_[i], meshSprites_[i + 1]);
						else if (currentType_ == Type::TEXT_NODE)
							nctl::swap(textNodes_[i], textNodes_[i + 1]);
						else if (currentType_ == Type::PARTICLE_SYSTEM)
							nctl::swap(particleSystems_[i], particleSystems_[i + 1]);
					}
				}
			}

			ImGui::PushItemWidth(150.0f);
			int imguiLayer = nc::theApplication().guiSettings().imguiLayer;
			ImGui::InputInt("ImGui layer", &imguiLayer);
			if (imguiLayer < 0)
				imguiLayer = 0;
			else if (imguiLayer > 0xFFFF)
				imguiLayer = 0xFFFF;
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button("Reset##ImGuiLayer"))
				imguiLayer = DefaultImGuiLayer;
			nc::theApplication().guiSettings().imguiLayer = imguiLayer;

			ImGui::Separator();
			ImGui::Checkbox("Overlapping", &overlapping_);
			ImGui::SameLine();
			ImGui::Checkbox("Culling", &settings.cullingEnabled);
			ImGui::SameLine();
			ImGui::Checkbox("Batching", &settings.batchingEnabled);
		}
		ImGui::End();
	}

	const float nodeWidth = (currentType_ != Type::PARTICLE_SYSTEM) ? static_cast<nc::DrawableNode *>(castToNode(0))->width() : sprites_[0]->width();
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		const float positionX = overlapping_
		                            ? (position_.x + i * nodeWidth + width - (NumSprites - 0.5f) * 0.5f * nodeWidth) * 0.5f
		                            : position_.x + width / (NumSprites + 3) * (i + 2);

		if (currentType_ == Type::SPRITE)
		{
			sprites_[i]->setAnchorPoint(anchorPoint_);
			sprites_[i]->setPositionX(positionX);
			sprites_[i]->setPositionY(position_.y + (height * 0.5f));
			sprites_[i]->setRotation(angle_);
			sprites_[i]->setScale(scale_);
			sprites_[i]->setFlippedX(flippedX_);
			sprites_[i]->setFlippedY(flippedY_);
			sprites_[i]->setColorF(color_);
			sprites_[i]->setBlendingEnabled(blendingEnabled_);
			sprites_[i]->setBlendingPreset(blendingPreset_);
		}
		else if (currentType_ == Type::MESH_SPRITE)
		{
			meshSprites_[i]->setAnchorPoint(anchorPoint_);
			meshSprites_[i]->setPositionX(positionX);
			meshSprites_[i]->setPositionY(position_.y + (height * 0.5f));
			meshSprites_[i]->setRotation(angle_);
			meshSprites_[i]->setScale(scale_);
			meshSprites_[i]->setFlippedX(flippedX_);
			meshSprites_[i]->setFlippedY(flippedY_);
			meshSprites_[i]->setColorF(color_);
			meshSprites_[i]->setBlendingEnabled(blendingEnabled_);
			meshSprites_[i]->setBlendingPreset(blendingPreset_);
		}
		else if (currentType_ == Type::TEXT_NODE)
		{
			textNodes_[i]->setAnchorPoint(anchorPoint_);
			textNodes_[i]->setPositionX(positionX);
			textNodes_[i]->setPositionY(position_.y + (height * 0.5f));
			textNodes_[i]->setRotation(angle_);
			textNodes_[i]->setScale(scale_);
			textNodes_[i]->setColorF(color_);
			textNodes_[i]->setBlendingEnabled(blendingEnabled_);
			textNodes_[i]->setBlendingPreset(blendingPreset_);
		}
		else if (currentType_ == Type::PARTICLE_SYSTEM)
		{
			particleSystems_[i]->setAnchorPoint(anchorPoint_);
			particleSystems_[i]->setPositionX(positionX);
			particleSystems_[i]->setPositionY(position_.y + (height * 0.5f));
			rotationAffectors_[i]->steps()[1].angle = angle_;
			sizeAffectors_[i]->setBaseScale(scale_);
			particleSystems_[i]->setFlippedX(flippedX_);
			particleSystems_[i]->setFlippedY(flippedY_);
			particleSystems_[i]->setColorF(color_);
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
	if (event.mod & nc::KeyMod::CTRL && event.sym == nc::KeySym::H)
		showImGui = !showImGui;
	else if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}

nc::SceneNode *MyEventHandler::castToNode(unsigned int index)
{
	nc::SceneNode *node = nullptr;

	if (index >= 0 && index < NumSprites)
	{
		if (currentType_ == Type::SPRITE)
			node = sprites_[index].get();
		else if (currentType_ == Type::MESH_SPRITE)
			node = meshSprites_[index].get();
		else if (currentType_ == Type::TEXT_NODE)
			node = textNodes_[index].get();
		else if (currentType_ == Type::PARTICLE_SYSTEM)
			node = particleSystems_[index].get();
	}

	return node;
}
