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
			UNSPECIFIED = 0,
			SPRITE,
			MESH_SPRITE,
			PARTICLE,
			TEXT,
#ifdef WITH_IMGUI
			IMGUI,
#endif
#ifdef WITH_NUKLEAR
			NUKLEAR,
#endif

			COUNT
		};
	};

	explicit RenderCommand(CommandTypes::Enum profilingType);
	RenderCommand();

	/// Returns the command type for profiling counter
	inline CommandTypes::Enum profilingType() const { return profilingType_; }
	/// Sets the command type for profiling counter
	inline void setProfilingType(CommandTypes::Enum profilingType) { profilingType_ = profilingType; }

	/// Returns the rendering layer
	inline unsigned short layer() const { return layer_; }
	/// Sets the rendering layer
	inline void setLayer(unsigned short layer) { layer_ = layer; }

	/// Returns the number of instances collected in the command or zero if instancing is not used
	inline int numInstances() const { return numInstances_; }
	/// Sets the number of instances collected in the command
	inline void setNumInstances(int numInstances) { numInstances_ = numInstances; }

	/// Returns the number of elements collected by the command or zero if it's not a batch
	inline int batchSize() const { return batchSize_; }
	/// Sets the number of batch elements collected by the command
	inline void setBatchSize(int batchSize) { batchSize_ = batchSize; }

	/// Commits all the uniform blocks of command's shader program
	void commitUniformBlocks();

	/// Returns the material sort key for the queue
	inline uint64_t materialSortKey() const { return materialSortKey_; }
	/// Calculates a material sort key for the queue
	void calculateMaterialSortKey();
	/// Returns the id based secondary sort key for the queue
	inline unsigned int idSortKey() const { return idSortKey_; }
	/// Sets the id based secondary sort key for the queue
	void setIdSortKey(unsigned int idSortKey) { idSortKey_ = idSortKey; }

	/// Issues the render command
	void issue();

	/// Gets the command type (for profiling purposes)
	inline CommandTypes::Enum type() const { return profilingType_; }
	/// Sets the command type (for profiling purposes)
	inline void setType(CommandTypes::Enum type) { profilingType_ = type; }

	void setScissor(GLint x, GLint y, GLsizei width, GLsizei height);

	inline Matrix4x4f &transformation() { return modelView_; }
	inline const Material &material() const { return material_; }
	inline const Geometry &geometry() const { return geometry_; }
	inline Material &material() { return material_; }
	inline Geometry &geometry() { return geometry_; }

	/// Commits the modelview matrix uniform
	void commitTransformation();

	/// Copy the vertices stored in host memory to video memory
	/*! This step is not needed if the command uses a custom VBO
	 * or directly writes into the common one */
	void commitVertices();

	/// Copy the indices stored in host memory to video memory
	/*! This step is not needed if the command uses a custom IBO
	 * or directly writes into the common one */
	void commitIndices();

  private:
	struct ScissorState
	{
		ScissorState()
		    : x(0), y(0), width(0), height(0) {}

		GLint x;
		GLint y;
		GLsizei width;
		GLsizei height;
	};

	/// The material sort key minimizes state changes when rendering commands
	uint64_t materialSortKey_;
	/// The id based secondary sort key stabilizes render commands sorting
	unsigned int idSortKey_;
	unsigned short layer_;
	int numInstances_;
	int batchSize_;
	bool uniformBlocksCommitted_;
	bool verticesCommitted_;
	bool indicesCommitted_;

	/// Command type for profiling counter
	CommandTypes::Enum profilingType_;

	ScissorState scissor_;

	Matrix4x4f modelView_;
	Material material_;
	Geometry geometry_;
};

}

#endif
