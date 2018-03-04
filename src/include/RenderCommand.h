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
	/*! Its sole purpose is to allow separated profiling counters in the `RenderStatistics` class. */
	struct CommandTypes
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

	RenderCommand(CommandTypes::Enum profilingType);
	RenderCommand();

	/// Returns the rendering layer
	inline unsigned int layer() const { return layer_; }
	/// Sets the rendering layer
	inline void setLayer(unsigned int layer) { ASSERT(layer <= TopLayer); layer_ = layer; }

	/// Returns the number of instances collected in the command or zero if it's not a batch
	inline int numInstances() const { return numInstances_; }
	/// Sets the number of instances in the batch collected in the command
	inline void setNumInstances(int numInstances) { numInstances_ = numInstances; }

	/// Commits all the uniform blocks of command's shader program
	void commitUniformBlocks();

	/// Returns the queue sort key
	inline unsigned long int sortKey() const { return sortKey_; }
	/// Calculates a sort key for the queue
	void calculateSortKey();
	/// Issues the render command
	void issue();

	/// Gets the command type (for profiling purposes)
	inline CommandTypes::Enum type() const { return profilingType_; }
	/// Sets the command type (for profiling purposes)
	inline void setType(CommandTypes::Enum type) { profilingType_ = type; }

	inline Matrix4x4f &transformation() { return modelView_; }
	inline const Material &material() const { return material_; }
	inline const Geometry &geometry() const { return geometry_; }
	inline Material &material() { return material_; }
	inline Geometry &geometry() { return geometry_; }
	void setVertexAttribute(const char *name, GLsizei vboStride, const GLvoid *vboPointer);

	void commitTransformation();

  private:
	unsigned long int sortKey_;
	unsigned int layer_;
	int numInstances_;
	bool uniformBlocksCommitted_;

	/// Command type for profiling counter
	CommandTypes::Enum profilingType_;

	Matrix4x4f modelView_;
	Material material_;
	Geometry geometry_;
};

}

#endif
