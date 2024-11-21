#ifndef CLASS_NCINE_PARTICLEAFFECTORS
#define CLASS_NCINE_PARTICLEAFFECTORS

#include "Vector2.h"
#include "Colorf.h"
#include <nctl/Array.h>

namespace ncine {

class Particle;

const unsigned int StepsInitialSize = 4;

/// Base class for particle affectors
/*! Affectors modify particle properties depending on their remaining life */
class DLL_PUBLIC ParticleAffector
{
  public:
	enum class Type
	{
		COLOR,
		SIZE,
		ROTATION,
		POSITION,
		VELOCITY
	};

	ParticleAffector(Type type)
	    : type_(type), enabled_(true) {}
	virtual ~ParticleAffector() {}

	/// Affects a property of the specified particle
	void affect(Particle *particle);
	/// Affects a property of the specified particle, without calculating the normalized age
	virtual void affect(Particle *particle, float normalizedAge) = 0;

	/// Returns the affector type
	inline Type type() const { return type_; }

	/// Returns true if the affector is enabled
	inline bool isEnabled() const { return enabled_; }
	/// Enables or disables the affector
	inline void setEnabled(bool enabled) { enabled_ = enabled; }

	/// Returns the number of steps
	virtual unsigned int numSteps() const = 0;
	/// Removes the step at the specified position index
	virtual void removeStep(unsigned int index) = 0;
	/// Removes all steps
	virtual void clearSteps() = 0;

  protected:
	/// Affector type
	Type type_;
	/// A flag indicating whether the affector is enabled or not
	bool enabled_;

	/// Protected default copy constructor used to clone objects
	ParticleAffector(const ParticleAffector &other) = default;
};

/// Particle color affector
class DLL_PUBLIC ColorAffector : public ParticleAffector
{
  public:
	struct ColorStep
	{
		float age;
		Colorf color;

		ColorStep()
		    : age(0.0f) {}
		ColorStep(float newAge, const Colorf &newColor)
		    : age(newAge), color(newColor) {}
	};

	ColorAffector()
	    : ParticleAffector(Type::COLOR), colorSteps_(StepsInitialSize) {}

	/// Default move constructor
	ColorAffector(ColorAffector &&) = default;
	/// Default move assignment operator
	ColorAffector &operator=(ColorAffector &&) = default;

	/// Returns a copy of this object
	inline ColorAffector clone() const { return ColorAffector(*this); }

	/// Affects the color of the specified particle
	void affect(Particle *particle, float normalizedAge) override;
	void addColorStep(float age, const Colorf &color);
	inline void addColorStep(const ColorStep &step) { addColorStep(step.age, step.color); }

	inline unsigned int numSteps() const override { return colorSteps_.size(); }
	void removeStep(unsigned int index) override;
	inline void clearSteps() override { colorSteps_.clear(); }

	inline nctl::Array<ColorStep> &steps() { return colorSteps_; }
	inline const nctl::Array<ColorStep> &steps() const { return colorSteps_; }

  protected:
	/// Protected default copy constructor used to clone objects
	ColorAffector(const ColorAffector &other) = default;

  private:
	nctl::Array<ColorStep> colorSteps_;
};

/// Particle size affector
class DLL_PUBLIC SizeAffector : public ParticleAffector
{
  public:
	struct SizeStep
	{
		float age;
		Vector2f scale;

		SizeStep()
		    : age(0.0f), scale(1.0f, 1.0f) {}
		SizeStep(float newAge, float newScale)
		    : age(newAge), scale(newScale, newScale) {}
		SizeStep(float newAge, float newScaleX, float newScaleY)
		    : age(newAge), scale(newScaleX, newScaleY) {}
		SizeStep(float newAge, const Vector2f &newScale)
		    : age(newAge), scale(newScale) {}
	};

	/// Constructs a size affector with a default base scale factor
	SizeAffector()
	    : SizeAffector(1.0f, 1.0f) {}
	/// Constructs a size affector with a base scale factor as a reference
	explicit SizeAffector(float baseScale)
	    : SizeAffector(baseScale, baseScale) {}
	/// Constructs a size affector with a horizontal and a vertical base scale factor as a reference
	SizeAffector(float baseScaleX, float baseScaleY)
	    : ParticleAffector(Type::SIZE), sizeSteps_(StepsInitialSize), baseScale_(baseScaleX, baseScaleY) {}
	/// Constructs a size affector with a vector base scale factor as a reference
	explicit SizeAffector(const Vector2f &baseScale)
	    : SizeAffector(baseScale.x, baseScale.y) {}

	/// Default move constructor
	SizeAffector(SizeAffector &&) = default;
	/// Default move assignment operator
	SizeAffector &operator=(SizeAffector &&) = default;

	/// Returns a copy of this object
	inline SizeAffector clone() const { return SizeAffector(*this); }

	/// Affects the size of the specified particle
	void affect(Particle *particle, float normalizedAge) override;
	inline void addSizeStep(float age, float scale) { addSizeStep(age, scale, scale); }
	void addSizeStep(float age, float scaleX, float scaleY);
	inline void addSizeStep(float age, const Vector2f &scale) { addSizeStep(age, scale.x, scale.y); }
	inline void addSizeStep(const SizeStep &step) { addSizeStep(step.age, step.scale); }

	inline unsigned int numSteps() const override { return sizeSteps_.size(); }
	void removeStep(unsigned int index) override;
	inline void clearSteps() override { sizeSteps_.clear(); }

	inline nctl::Array<SizeStep> &steps() { return sizeSteps_; }
	inline const nctl::Array<SizeStep> &steps() const { return sizeSteps_; }

	inline float baseScaleX() const { return baseScale_.x; }
	inline void setBaseScaleX(float baseScaleX) { baseScale_.x = baseScaleX; }
	inline float baseScaleY() const { return baseScale_.y; }
	inline void setBaseScaleY(float baseScaleY) { baseScale_.y = baseScaleY; }

	/// Returns the base scale factor that will be multiplied by all steps
	inline const Vector2f &baseScale() const { return baseScale_; }
	/// Sets a base scale factor that will be multiplied by all steps
	inline void setBaseScale(float baseScale) { baseScale_.set(baseScale, baseScale); }
	/// Sets a base scale factor with a `Vector2f`, that will be multiplied by all steps
	inline void setBaseScale(const Vector2f &baseScale) { baseScale_ = baseScale; }

  protected:
	/// Protected default copy constructor used to clone objects
	SizeAffector(const SizeAffector &other) = default;

  private:
	nctl::Array<SizeStep> sizeSteps_;
	Vector2f baseScale_;
};

/// Particle rotation affector
class DLL_PUBLIC RotationAffector : public ParticleAffector
{
  public:
	struct RotationStep
	{
		float age;
		float angle;

		RotationStep()
		    : age(0.0f), angle(0.0f) {}
		RotationStep(float newAge, float newAngle)
		    : age(newAge), angle(newAngle) {}
	};

	RotationAffector()
	    : ParticleAffector(Type::ROTATION), rotationSteps_(StepsInitialSize) {}

	/// Default move constructor
	RotationAffector(RotationAffector &&) = default;
	/// Default move assignment operator
	RotationAffector &operator=(RotationAffector &&) = default;

	/// Returns a copy of this object
	inline RotationAffector clone() const { return RotationAffector(*this); }

	/// Affects the rotation of the specified particle
	void affect(Particle *particle, float normalizedAge) override;
	void addRotationStep(float age, float angle);
	inline void addRotationStep(const RotationStep &step) { addRotationStep(step.age, step.angle); }

	inline unsigned int numSteps() const override { return rotationSteps_.size(); }
	void removeStep(unsigned int index) override;
	inline void clearSteps() override { rotationSteps_.clear(); }

	inline nctl::Array<RotationStep> &steps() { return rotationSteps_; }
	inline const nctl::Array<RotationStep> &steps() const { return rotationSteps_; }

  protected:
	/// Protected default copy constructor used to clone objects
	RotationAffector(const RotationAffector &other) = default;

  private:
	nctl::Array<RotationStep> rotationSteps_;
};

/// Particle position affector
class DLL_PUBLIC PositionAffector : public ParticleAffector
{
  public:
	struct PositionStep
	{
		float age;
		Vector2f position;

		PositionStep()
		    : age(0.0f), position(0.0f, 0.0f) {}
		PositionStep(float newAge, float newPositionX, float newPositionY)
		    : age(newAge), position(newPositionX, newPositionY) {}
	};

	PositionAffector()
	    : ParticleAffector(Type::POSITION), positionSteps_(StepsInitialSize) {}

	/// Default move constructor
	PositionAffector(PositionAffector &&) = default;
	/// Default move assignment operator
	PositionAffector &operator=(PositionAffector &&) = default;

	/// Returns a copy of this object
	inline PositionAffector clone() const { return PositionAffector(*this); }

	/// Affects the position of the specified particle
	void affect(Particle *particle, float normalizedAge) override;
	void addPositionStep(float age, float posX, float posY);
	inline void addPositionStep(float age, const Vector2f &position) { addPositionStep(age, position.x, position.y); }
	inline void addPositionStep(const PositionStep &step) { addPositionStep(step.age, step.position); }

	inline unsigned int numSteps() const override { return positionSteps_.size(); }
	void removeStep(unsigned int index) override;
	inline void clearSteps() override { positionSteps_.clear(); }

	inline nctl::Array<PositionStep> &steps() { return positionSteps_; }
	inline const nctl::Array<PositionStep> &steps() const { return positionSteps_; }

  protected:
	/// Protected default copy constructor used to clone objects
	PositionAffector(const PositionAffector &other) = default;

  private:
	nctl::Array<PositionStep> positionSteps_;
};

/// Particle velocity affector
class DLL_PUBLIC VelocityAffector : public ParticleAffector
{
  public:
	struct VelocityStep
	{
		float age;
		Vector2f velocity;

		VelocityStep()
		    : age(0.0f), velocity(0.0f, 0.0f) {}
		VelocityStep(float newAge, float newVelocityX, float newVelocityY)
		    : age(newAge), velocity(newVelocityX, newVelocityY) {}
	};

	VelocityAffector()
	    : ParticleAffector(Type::VELOCITY), velocitySteps_(StepsInitialSize) {}

	/// Default move constructor
	VelocityAffector(VelocityAffector &&) = default;
	/// Default move assignment operator
	VelocityAffector &operator=(VelocityAffector &&) = default;

	/// Returns a copy of this object
	inline VelocityAffector clone() const { return VelocityAffector(*this); }

	/// Affects the velocity of the specified particle
	void affect(Particle *particle, float normalizedAge) override;
	void addVelocityStep(float age, float velX, float velY);
	inline void addVelocityStep(float age, const Vector2f &velocity) { addVelocityStep(age, velocity.x, velocity.y); }
	inline void addVelocityStep(const VelocityStep &step) { addVelocityStep(step.age, step.velocity); }

	inline unsigned int numSteps() const override { return velocitySteps_.size(); }
	void removeStep(unsigned int index) override;
	inline void clearSteps() override { velocitySteps_.clear(); }

	inline nctl::Array<VelocityStep> &steps() { return velocitySteps_; }
	inline const nctl::Array<VelocityStep> &steps() const { return velocitySteps_; }

  protected:
	/// Protected default copy constructor used to clone objects
	VelocityAffector(const VelocityAffector &other) = default;

  private:
	nctl::Array<VelocityStep> velocitySteps_;
};

}

#endif
