#ifndef CLASS_NCINE_RENDERCOMMAND
#define CLASS_NCINE_RENDERCOMMAND

#include "Matrix4x4.h"
#include "Material.h"
#include "Geometry.h"
#include "Texture.h"

namespace ncine {

class Color;

/// The class wrapping all the information needed for issuing a draw command
class RenderCommand
{
  public:
	/// Command types
	/*! Its sole purpose is to allow separated profiling counters in the `RenderQueue` class. */
	struct CommandType
	{
		enum Enum
		{
			GENERIC = 0,
			PLOTTER,
			SPRITE,
			PARTICLE,
			TEXT,

			COUNT
		};
	};

	/// Bottom rendering layer limit
	static const unsigned int BottomLayer = 0;
	/// Top rendering layer limit
	static const unsigned int TopLayer = 65535;

	RenderCommand();

	/// Returns the rendering layer
	inline unsigned int layer() const { return layer_; }
	/// Sets the rendering layer
	inline void setLayer(unsigned int layer) { ASSERT(layer <= TopLayer); layer_ = layer; }

	/// Returns the queue sort key
	inline unsigned long int sortKey() const { return sortKey_; }
	/// Calculates a sort key for the queue
	void calculateSortKey();
	/// Issues the render command
	void issue();

	/// Gets the command type (for profiling purposes)
	inline CommandType::Enum type() const { return profilingType_; }
	/// Sets the command type (for profiling purposes)
	inline void setType(CommandType::Enum type) { profilingType_ = type; }

	inline Matrix4x4f &transformation() { return modelView_; }
	inline const Material &material() const { return material_; }
	inline const Geometry &geometry() const { return geometry_; }
	inline Material &material() { return material_; }
	inline Geometry &geometry() { return geometry_; }
	void setVertexAttribute(const char *name, GLsizei vboStride, const GLvoid *vboPointer);

  private:
	unsigned long int sortKey_;
	unsigned int layer_;
	/// Command type for profiling counter
	CommandType::Enum profilingType_;

	Matrix4x4f modelView_;
	Material material_;
	Geometry geometry_;

	void setTransformation();
	void draw();
};

}

#endif
