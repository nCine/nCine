#ifndef CLASS_NCINE_RENDERSTATISTICS
#define CLASS_NCINE_RENDERSTATISTICS

#include "nctl/String.h"
#include "RenderCommand.h"

namespace ncine {

/// A class to gather statistics about the rendering subsystem
class RenderStatistics
{
  public:
	class Commands
	{
	  public:
		unsigned int vertices;
		unsigned int commands;
		unsigned int transparents;
		unsigned int instances;
		unsigned int batchSize;

		Commands() : vertices(0), commands(0), transparents(0), instances(0), batchSize(0) { }

	  private:
		void reset() { vertices = 0; commands = 0; transparents = 0; instances = 0; batchSize = 0; }
		friend RenderStatistics;
	};

	class Buffers
	{
	  public:
		unsigned int count;
		unsigned long size;
		unsigned long usedSpace;

		Buffers() : count(0), size(0), usedSpace(0) { }

	  private:
		void reset() { count = 0; size = 0; usedSpace = 0; }
		friend RenderStatistics;
	};

	class Textures
	{
	  public:
		unsigned int count;
		unsigned long dataSize;

		Textures() : count(0), dataSize(0) { }

	  private:
		void reset() { count = 0; dataSize = 0; }
		friend RenderStatistics;
	};

	class CustomVbos
	{
	  public:
		unsigned int count;
		unsigned long dataSize;

		CustomVbos() : count(0), dataSize(0) { }

	  private:
		void reset() { count = 0; dataSize = 0; }
		friend RenderStatistics;
	};

	class VaoPool
	{
	  public:
		unsigned int size;
		unsigned int capacity;
		unsigned int reuses;
		unsigned int bindings;

		VaoPool() : size(0), capacity(0), reuses(0), bindings(0) { }

	  private:
		void reset() { size = 0; capacity = 0; reuses = 0; bindings = 0; }
		friend RenderStatistics;
	};

	/// Returns the aggregated command statistics for all types
	static inline const Commands &allCommands() { return allCommands_; }
	/// Returns the commnad statistics for the specified type
	static inline const Commands &commands(RenderCommand::CommandTypes::Enum type) { return typedCommands_[type]; }

	/// Returns the buffer statistics for the specified type
	static inline const Buffers &buffers(RenderBuffersManager::BufferTypes::Enum type) { return typedBuffers_[type]; }

	/// Returns aggregated texture statistics
	static inline const Textures &textures() { return textures_; }

	/// Returns aggregated custom VBOs statistics
	static inline const CustomVbos &customVBOs() { return customVbos_; }

	/// Returns the number of `DrawableNodes` culled because outside of the screen
	static inline unsigned int culled() { return culledNodes_[(index + 1) % 2]; }

	/// Appends commands statistics to the string passed as argument
	static void appendCommandsStatistics(nctl::String &string);
	/// Appends additional statistics to the string passed as argument
	static void appendMoreStatistics(nctl::String &string);

  private:
	/// The string used to output OpenGL debug group information
	static nctl::String debugString_;

	static Commands allCommands_;
	static Commands typedCommands_[RenderCommand::CommandTypes::COUNT];
	static Buffers typedBuffers_[RenderBuffersManager::BufferTypes::COUNT];
	static Textures textures_;
	static CustomVbos customVbos_;
	static unsigned int index;
	static unsigned int culledNodes_[2];
	static VaoPool vaoPool_;

	static void reset();
	static void gatherStatistics(const RenderCommand &command);
	static void gatherStatistics(const RenderBuffersManager::ManagedBuffer &buffer);
	static inline void gatherVaoPoolStatistics(unsigned int poolSize, unsigned int poolCapacity) { vaoPool_.size = poolSize; vaoPool_.capacity = poolCapacity; }
	static inline void addTexture(unsigned long datasize) { textures_.count++; textures_.dataSize += datasize; }
	static inline void removeTexture(unsigned long datasize) { textures_.count--; textures_.dataSize -= datasize; }
	static inline void addCustomVbo(unsigned long datasize) { customVbos_.count++; customVbos_.dataSize += datasize; }
	static inline void removeCustomVbo(unsigned long datasize) { customVbos_.count--; customVbos_.dataSize -= datasize; }
	static inline void addCulledNode() { culledNodes_[index]++; }
	static inline void addVaoPoolReuse() { vaoPool_.reuses++; }
	static inline void addVaoPoolBinding() { vaoPool_.bindings++; }

	friend class RenderQueue;
	friend class RenderBuffersManager;
	friend class Texture;
	friend class Geometry;
	friend class DrawableNode;
	friend class RenderVaoPool;
};

}

#endif
