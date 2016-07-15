#ifndef CLASS_NCINE_RENDERCOMMAND
#define CLASS_NCINE_RENDERCOMMAND

#include "Vector2.h"
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
	enum CommandType
	{
		GENERIC_TYPE = 0,
		PLOTTER_TYPE,
		SPRITE_TYPE,
		PARTICLE_TYPE,
		TEXT_TYPE,
		TYPE_COUNT
	};

	/// Rendering layer limits
	enum LayerLimits
	{
		BOTTOM_LAYER = 0,
		TOP_LAYER = 65535
	};

	RenderCommand();

	/// Returns the rendering layer
	inline unsigned int layer() const { return layer_; }
	/// Sets the rendering layer
	inline void setLayer(unsigned int layer) { layer_ = (layer < static_cast<unsigned int>(TOP_LAYER)) ? layer : static_cast<unsigned int>(TOP_LAYER); }

	/// Returns the queue sort key
	inline unsigned long int sortKey() const { return sortKey_; }
	/// Calculates a sort key for the queue
	void calculateSortKey();
	/// Issues the render command
	void issue();

	/// Gets the command type (for profiling purposes)
	inline CommandType type() const { return profilingType_; }
	/// Sets the command type (for profiling purposes)
	inline void setType(CommandType eType) { profilingType_ = eType; }

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
	CommandType profilingType_;

	Matrix4x4f modelView_;
	Material material_;
	Geometry geometry_;

	void setTransformation();
	void draw();
};

}

#endif
