#include <ncine/config.h>

#include "apptest_particles.h"
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/ParticleSystem.h>
#include <ncine/ParticleInitializer.h>
#include "apptest_datapath.h"

#ifdef __ANDROID__
	#include <ncine/AndroidApplication.h>
#endif

#if NCINE_WITH_IMGUI
	#include <ncine/imgui.h>
	#include <ncine/ParticleAffectors.h>
#endif

namespace {

#ifdef __ANDROID__
const char *TextureFile = "smoke_256_ETC2.ktx";
#else
const char *TextureFile = "smoke_256.png";
#endif

const float KeySpeed = 200.0f;
const float JoySpeed = 200.0f;
nc::ParticleInitializer particleInit;
float emitTime = 0.085f;
bool autoEmit = true;

bool captureMouse = true;
#if NCINE_WITH_IMGUI
const float LineHeightSpacing = 0.5f;
bool showImGui = true;

struct HasAffector
{
	bool color = false;
	bool size = false;
	bool rotation = false;
	bool position = false;
	bool velocity = false;
} hasAffector;

bool removeAffectorType(nc::ParticleAffector::Type type, nc::ParticleSystem &particleSystem)
{
	int removeIndex = -1;
	for (unsigned int i = 0; i < particleSystem.affectors().size(); i++)
	{
		if (particleSystem.affectors()[i]->type() == type)
		{
			removeIndex = i;
			break;
		}
	}

	if (removeIndex >= 0)
		particleSystem.affectors().removeAt(removeIndex);

	return (removeIndex >= 0);
}
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
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

#ifdef __ANDROID__
	nc::AndroidApplication &application = static_cast<nc::AndroidApplication &>(nc::theApplication());
	application.enableAccelerometer(true);
#endif

	texture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", TextureFile)).data());
	particleSystem_ = nctl::makeUnique<nc::ParticleSystem>(&rootNode, unsigned(NumParticles), texture_.get(), texture_->rect());
	particleSystem_->setPosition(nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.33f);

	nctl::UniquePtr<nc::ColorAffector> colorAffector = nctl::makeUnique<nc::ColorAffector>();
	colorAffector->addColorStep(0.0f, nc::Colorf(0.0f, 0.0f, 1.0f, 0.9f));
	colorAffector->addColorStep(0.3f, nc::Colorf(0.86f, 0.7f, 0.0f, 0.65f));
	colorAffector->addColorStep(0.35f, nc::Colorf(0.86f, 0.59f, 0.0f, 0.8f));
	colorAffector->addColorStep(1.0f, nc::Colorf(0.86f, 0.39f, 0.0f, 0.75f));
	particleSystem_->addAffector(nctl::move(colorAffector));
	nctl::UniquePtr<nc::SizeAffector> sizeAffector = nctl::makeUnique<nc::SizeAffector>(0.45f);
	sizeAffector->addSizeStep(0.0f, 0.4f);
	sizeAffector->addSizeStep(0.3f, 1.7f);
	sizeAffector->addSizeStep(1.0f, 0.01f);
	particleSystem_->addAffector(nctl::move(sizeAffector));
	emitVector_.set(0.0f, 350.0f);
	prevEmitVector_ = emitVector_;

	particleInit.setAmount(3);
	particleInit.setLife(0.85f, 1.0f);
	particleInit.setPositionAndRadius(nc::Vector2f::Zero, 10.0f);
	particleInit.setVelocityAndScale(emitVector_, 0.8f, 1.0f);

	pause_ = false;
	lastEmissionTime_ = nc::TimeStamp::now();

	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}

void MyEventHandler::onFrameStart()
{
#if NCINE_WITH_IMGUI
	captureMouse = (ImGui::GetIO().WantCaptureMouse == false);
	if (showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(460.0f, 460.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(40.0f, 40.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("apptest_particles", &showImGui))
		{
			hasAffector = HasAffector();
			for (nctl::UniquePtr<nc::ParticleAffector> &affector : particleSystem_->affectors())
			{
				switch (affector->type())
				{
					case nc::ParticleAffector::Type::COLOR:
					{
						nc::ColorAffector *colorAffector = reinterpret_cast<nc::ColorAffector *>(affector.get());
						if (ImGui::TreeNode("Color affector"))
						{
							bool enabled = colorAffector->isEnabled();
							ImGui::Checkbox("Enabled", &enabled);
							colorAffector->setEnabled(enabled);

							int removeIndex = -1;
							for (unsigned int i = 0; i < colorAffector->steps().size(); i++)
							{
								nc::ColorAffector::ColorStep &step = colorAffector->steps()[i];

								ImGui::PushID(&step);
								ImGui::SliderFloat("Age", &step.age, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
								ImGui::ColorEdit3("Color", step.color.data());
								ImGui::SameLine();
								if (ImGui::Button("[-]"))
									removeIndex = i;
								ImGui::PopID();
								ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * LineHeightSpacing));

								if (i < colorAffector->steps().size() - 1 && step.age > colorAffector->steps()[i + 1].age)
									nctl::swap(step.age, colorAffector->steps()[i + 1].age);
							}

							if (removeIndex >= 0)
								colorAffector->steps().removeAt(removeIndex);

							ImGui::Separator();
							static nc::ColorAffector::ColorStep newStep;
							ImGui::SliderFloat("Age", &newStep.age, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							ImGui::ColorEdit3("Color", newStep.color.data());
							ImGui::SameLine();
							if (ImGui::Button("[+]"))
								colorAffector->addColorStep(newStep);

							ImGui::TreePop();
						}
						hasAffector.color = true;
						break;
					}
					case nc::ParticleAffector::Type::SIZE:
					{
						nc::SizeAffector *sizeAffector = reinterpret_cast<nc::SizeAffector *>(affector.get());
						if (ImGui::TreeNode("Size affector"))
						{
							bool enabled = sizeAffector->isEnabled();
							ImGui::Checkbox("Enabled", &enabled);
							sizeAffector->setEnabled(enabled);

							int removeIndex = -1;
							nc::Vector2f baseScale(sizeAffector->baseScale());
							if (ImGui::SliderFloat2("Base scale", baseScale.data(), 0.0f, 2.0f, "%.2f"))
								sizeAffector->setBaseScale(baseScale);
							ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * LineHeightSpacing));

							for (unsigned int i = 0; i < sizeAffector->steps().size(); i++)
							{
								nc::SizeAffector::SizeStep &step = sizeAffector->steps()[i];

								ImGui::PushID(&step);
								ImGui::SliderFloat("Age", &step.age, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
								ImGui::SliderFloat2("Scale", step.scale.data(), 0.0f, 2.0f, "%.2f");
								ImGui::SameLine();
								if (ImGui::Button("[-]"))
									removeIndex = i;
								ImGui::PopID();
								ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * LineHeightSpacing));

								if (i < sizeAffector->steps().size() - 1 && step.age > sizeAffector->steps()[i + 1].age)
									nctl::swap(step.age, sizeAffector->steps()[i + 1].age);
							}

							if (removeIndex >= 0)
								sizeAffector->steps().removeAt(removeIndex);

							ImGui::Separator();
							static nc::SizeAffector::SizeStep newStep;
							ImGui::SliderFloat("Age", &newStep.age, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							ImGui::SliderFloat2("Scale", newStep.scale.data(), 0.0f, 2.0f, "%.2f");
							ImGui::SameLine();
							if (ImGui::Button("[+]"))
								sizeAffector->addSizeStep(newStep);

							ImGui::TreePop();
						}
						hasAffector.size = true;
						break;
					}
					case nc::ParticleAffector::Type::ROTATION:
					{
						nc::RotationAffector *rotationAffector = reinterpret_cast<nc::RotationAffector *>(affector.get());
						if (ImGui::TreeNode("Rotation affector"))
						{
							bool enabled = rotationAffector->isEnabled();
							ImGui::Checkbox("Enabled", &enabled);
							rotationAffector->setEnabled(enabled);

							int removeIndex = -1;
							for (unsigned int i = 0; i < rotationAffector->steps().size(); i++)
							{
								nc::RotationAffector::RotationStep &step = rotationAffector->steps()[i];

								ImGui::PushID(&step);
								ImGui::SliderFloat("Age", &step.age, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
								ImGui::SliderFloat("Angle", &step.angle, 0.0f, 360.0f, "%.2f");
								ImGui::SameLine();
								if (ImGui::Button("[-]"))
									removeIndex = i;
								ImGui::PopID();
								ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * LineHeightSpacing));

								if (i < rotationAffector->steps().size() - 1 && step.age > rotationAffector->steps()[i + 1].age)
									nctl::swap(step.age, rotationAffector->steps()[i + 1].age);
							}

							if (removeIndex >= 0)
								rotationAffector->steps().removeAt(removeIndex);

							ImGui::Separator();
							static nc::RotationAffector::RotationStep newStep;
							ImGui::SliderFloat("Age", &newStep.age, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							ImGui::SliderFloat("Angle", &newStep.angle, 0.0f, 360.0f, "%.2f");
							ImGui::SameLine();
							if (ImGui::Button("[+]"))
								rotationAffector->addRotationStep(newStep);

							ImGui::TreePop();
						}
						hasAffector.rotation = true;
						break;
					}
					case nc::ParticleAffector::Type::POSITION:
					{
						nc::PositionAffector *positionAffector = reinterpret_cast<nc::PositionAffector *>(affector.get());
						if (ImGui::TreeNode("Position affector"))
						{
							bool enabled = positionAffector->isEnabled();
							ImGui::Checkbox("Enabled", &enabled);
							positionAffector->setEnabled(enabled);

							int removeIndex = -1;
							for (unsigned int i = 0; i < positionAffector->steps().size(); i++)
							{
								nc::PositionAffector::PositionStep &step = positionAffector->steps()[i];

								ImGui::PushID(&step);
								ImGui::SliderFloat("Age", &step.age, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
								ImGui::InputFloat2("Position", step.position.data(), "%.2f");
								ImGui::SameLine();
								if (ImGui::Button("[-]"))
									removeIndex = i;
								ImGui::PopID();
								ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * LineHeightSpacing));

								if (i < positionAffector->steps().size() - 1 && step.age > positionAffector->steps()[i + 1].age)
									nctl::swap(step.age, positionAffector->steps()[i + 1].age);
							}

							if (removeIndex >= 0)
								positionAffector->steps().removeAt(removeIndex);

							ImGui::Separator();
							static nc::PositionAffector::PositionStep newStep;
							ImGui::SliderFloat("Age", &newStep.age, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							ImGui::InputFloat2("Position", newStep.position.data(), "%.2f");
							ImGui::SameLine();
							if (ImGui::Button("[+]"))
								positionAffector->addPositionStep(newStep);

							ImGui::TreePop();
						}
						hasAffector.position = true;
						break;
					}
					case nc::ParticleAffector::Type::VELOCITY:
					{
						nc::VelocityAffector *velocityAffector = reinterpret_cast<nc::VelocityAffector *>(affector.get());
						if (ImGui::TreeNode("Velocity affector"))
						{
							bool enabled = velocityAffector->isEnabled();
							ImGui::Checkbox("Enabled", &enabled);
							velocityAffector->setEnabled(enabled);

							int removeIndex = -1;
							for (unsigned int i = 0; i < velocityAffector->steps().size(); i++)
							{
								nc::VelocityAffector::VelocityStep &step = velocityAffector->steps()[i];

								ImGui::PushID(&step);
								ImGui::SliderFloat("Age", &step.age, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
								ImGui::InputFloat2("Velocity", step.velocity.data(), "%.2f");
								ImGui::SameLine();
								if (ImGui::Button("[-]"))
									removeIndex = i;
								ImGui::PopID();
								ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * LineHeightSpacing));

								if (i < velocityAffector->steps().size() - 1 && step.age > velocityAffector->steps()[i + 1].age)
									nctl::swap(step.age, velocityAffector->steps()[i + 1].age);
							}

							if (removeIndex >= 0)
								velocityAffector->steps().removeAt(removeIndex);

							ImGui::Separator();
							static nc::VelocityAffector::VelocityStep newStep;
							ImGui::SliderFloat("Age", &newStep.age, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							ImGui::InputFloat2("Velocity", newStep.velocity.data(), "%.2f");
							ImGui::SameLine();
							if (ImGui::Button("[+]"))
								velocityAffector->addVelocityStep(newStep);

							ImGui::TreePop();
						}
						hasAffector.velocity = true;
						break;
					}
				}
			}

			if (hasAffector.color)
			{
				if (ImGui::Button("[-] Color"))
					removeAffectorType(nc::ParticleAffector::Type::COLOR, *particleSystem_);
			}
			else
			{
				if (ImGui::Button("[+] Color"))
				{
					nctl::UniquePtr<nc::ColorAffector> colorAffector = nctl::makeUnique<nc::ColorAffector>();
					particleSystem_->addAffector(nctl::move(colorAffector));
				}
			}

			ImGui::SameLine();
			if (hasAffector.size)
			{
				if (ImGui::Button("[-] Size"))
					removeAffectorType(nc::ParticleAffector::Type::SIZE, *particleSystem_);
			}
			else
			{
				if (ImGui::Button("[+] Size"))
				{
					nctl::UniquePtr<nc::SizeAffector> sizeAffector = nctl::makeUnique<nc::SizeAffector>();
					particleSystem_->addAffector(nctl::move(sizeAffector));
				}
			}

			ImGui::SameLine();
			if (hasAffector.rotation)
			{
				if (ImGui::Button("[-] Rotation"))
					removeAffectorType(nc::ParticleAffector::Type::ROTATION, *particleSystem_);
			}
			else
			{

				if (ImGui::Button("[+] Rotation"))
				{
					nctl::UniquePtr<nc::RotationAffector> rotationAffector = nctl::makeUnique<nc::RotationAffector>();
					particleSystem_->addAffector(nctl::move(rotationAffector));
				}
			}

			ImGui::SameLine();
			if (hasAffector.position)
			{
				if (ImGui::Button("[-] Position"))
					removeAffectorType(nc::ParticleAffector::Type::POSITION, *particleSystem_);
			}
			else
			{
				if (ImGui::Button("[+] Position"))
				{
					nctl::UniquePtr<nc::PositionAffector> positionAffector = nctl::makeUnique<nc::PositionAffector>();
					particleSystem_->addAffector(nctl::move(positionAffector));
				}
			}

			ImGui::SameLine();
			if (hasAffector.velocity)
			{
				if (ImGui::Button("[-] Velocity"))
					removeAffectorType(nc::ParticleAffector::Type::VELOCITY, *particleSystem_);
			}
			else
			{
				if (ImGui::Button("[+] Velocity"))
				{
					nctl::UniquePtr<nc::VelocityAffector> velocityAffector = nctl::makeUnique<nc::VelocityAffector>();
					particleSystem_->addAffector(nctl::move(velocityAffector));
				}
			}

			ImGui::Separator();

			nc::Vector2f position(particleSystem_->position());
			if (ImGui::InputFloat2("Position", position.data(), "%.2f"))
			if (ImGui::IsItemDeactivatedAfterEdit())
				particleSystem_->setPosition(position);

			nc::Vector2f tempEmitVector(emitVector_);
			if (ImGui::InputFloat2("Emit vector", tempEmitVector.data(), "%.2f"))
			if (ImGui::IsItemDeactivatedAfterEdit())
				emitVector_ = tempEmitVector;
			ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * LineHeightSpacing));

			ImGui::SliderInt2("Init amount", particleInit.rndAmount.data(), 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat2("Init life", particleInit.rndLife.data(), 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::InputFloat2("Init position X", particleInit.rndPositionX.data(), "%.2f");
			ImGui::InputFloat2("Init position Y", particleInit.rndPositionY.data(), "%.2f");
			ImGui::InputFloat2("Init velocity X", particleInit.rndVelocityX.data(), "%.2f");
			ImGui::InputFloat2("Init velocity Y", particleInit.rndVelocityY.data(), "%.2f");
			ImGui::SliderFloat2("Init rotation", particleInit.rndRotation.data(), 0.0f, 360.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Checkbox("Init emitter rotation", &particleInit.emitterRotation);
			particleInit.validateMinMax();
			ImGui::SameLine();
			bool inLocalSpace = particleSystem_->inLocalSpace();
			if (ImGui::Checkbox("Local space", &inLocalSpace))
				particleSystem_->setInLocalSpace(inLocalSpace);

			bool particlesUpdateEnabled = particleSystem_->isParticlesUpdateEnabled();
			if (ImGui::Checkbox("Particles update", &particlesUpdateEnabled))
				particleSystem_->setParticlesUpdateEnabled(particlesUpdateEnabled);
			pause_ = !particlesUpdateEnabled;
			ImGui::SameLine();
			bool affectorsEnabled = particleSystem_->areAffectorsEnabled();
			if (ImGui::Checkbox("Affectors enabled", &affectorsEnabled))
				particleSystem_->setAffectorsEnabled(affectorsEnabled);
			ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * LineHeightSpacing));

			ImGui::SliderFloat("Emit time", &emitTime, 0.0f, 1.0f, "%.3f s", ImGuiSliderFlags_AlwaysClamp);
			if (ImGui::Button("Kill particles"))
				particleSystem_->killParticles();
			ImGui::SameLine();
			if (ImGui::Button("Emit particles"))
				particleSystem_->emitParticles(particleInit);
			ImGui::SameLine();
			ImGui::Checkbox("Auto emit", &autoEmit);

			const float aliveFraction = particleSystem_->numAliveParticles() / static_cast<float>(particleSystem_->numParticles());
			ImGui::ProgressBar(aliveFraction, ImVec2(0.0f, 0.0f));
			ImGui::Text("Particles: %d / %d", particleSystem_->numAliveParticles(), particleSystem_->numParticles());
		}
		ImGui::End();
	}
#endif

	const float frameTime = nc::theApplication().frameTime();

	if (prevEmitVector_.x != emitVector_.x || prevEmitVector_.y != emitVector_.y)
	{
		particleInit.setVelocity(emitVector_);
		prevEmitVector_ = emitVector_;
	}
	if (pause_ == false)
	{
		if (autoEmit && lastEmissionTime_.secondsSince() > emitTime)
		{
			lastEmissionTime_ = nc::TimeStamp::now();
			particleSystem_->emitParticles(particleInit);
		}
	}
	particleSystem_->setParticlesUpdateEnabled(!pause_);

	particleSystem_->move(joyVectorLeft_ * JoySpeed * frameTime);
	emitVector_ += joyVectorRight_ * JoySpeed * frameTime;

	const nc::KeyboardState &keyState = nc::theApplication().inputManager().keyboardState();

	if (keyState.isKeyDown(nc::KeySym::D))
		particleSystem_->moveX(KeySpeed * frameTime);
	else if (keyState.isKeyDown(nc::KeySym::A))
		particleSystem_->moveX(-KeySpeed * frameTime);
	if (keyState.isKeyDown(nc::KeySym::W))
		particleSystem_->moveY(KeySpeed * frameTime);
	else if (keyState.isKeyDown(nc::KeySym::S))
		particleSystem_->moveY(-KeySpeed * frameTime);

	if (keyState.isKeyDown(nc::KeySym::RIGHT))
		emitVector_.x += KeySpeed * frameTime;
	else if (keyState.isKeyDown(nc::KeySym::LEFT))
		emitVector_.x -= KeySpeed * frameTime;
	if (keyState.isKeyDown(nc::KeySym::UP))
		emitVector_.y += KeySpeed * frameTime;
	else if (keyState.isKeyDown(nc::KeySym::DOWN))
		emitVector_.y -= KeySpeed * frameTime;
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	if (captureMouse)
		particleSystem_->setPosition(event.pointers[0].x, event.pointers[0].y);
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	if (captureMouse)
	{
		particleSystem_->setPosition(event.pointers[0].x, event.pointers[0].y);

		if (event.count > 1)
		{
			emitVector_.x = (event.pointers[1].x - particleSystem_->position().x) * 2.5f;
			emitVector_.y = (event.pointers[1].y - particleSystem_->position().y) * 2.5f;
		}
	}
}

void MyEventHandler::onAcceleration(const nc::AccelerometerEvent &event)
{
	particleSystem_->moveX(event.y * 0.75f);
	particleSystem_->moveY(-event.x * 0.75f);
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::P)
		pause_ = !pause_;
#if NCINE_WITH_IMGUI
	else if (event.mod & nc::KeyMod::CTRL && event.sym == nc::KeySym::H)
		showImGui = !showImGui;
#endif
	else if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.button == nc::MouseButton::LEFT && captureMouse)
		particleSystem_->setPosition(static_cast<float>(event.x), static_cast<float>(event.y));
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (captureMouse)
	{
		if (state.isButtonDown(nc::MouseButton::LEFT))
			particleSystem_->setPosition(static_cast<float>(state.x), static_cast<float>(state.y));
		else if (state.isButtonDown(nc::MouseButton::RIGHT))
		{
			emitVector_.x = (state.x - particleSystem_->position().x) * 2.5f;
			emitVector_.y = (state.y - particleSystem_->position().y) * 2.5f;
		}
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

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();

	if (event.buttonName == nc::ButtonName::A)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.buttonName == nc::ButtonName::B)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
#if NCINE_WITH_IMGUI
	else if (event.buttonName == nc::ButtonName::BACK)
		showImGui = !showImGui;
#endif
	else if (event.buttonName == nc::ButtonName::START)
		pause_ = !pause_;
	else if (event.buttonName == nc::ButtonName::GUIDE)
		nc::theApplication().quit();
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}
