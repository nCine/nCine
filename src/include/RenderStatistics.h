#ifndef CLASS_NCINE_RENDERSTATISTICS
#define CLASS_NCINE_RENDERSTATISTICS

#include <nctl/String.h>
#include "RenderCommand.h"

namespace ncine {

/// A class to gather statistics about the rendering subsystem
class RenderStatistics
{
  public:
#ifdef WITH_SCENEGRAPH
	class Commands
	{
	  public:
		unsigned int vertices;
		unsigned int commands;
		unsigned int transparents;
		unsigned int instances;
		unsigned int batchSize;

		Commands()
		    : vertices(0), commands(0), transparents(0), instances(0), batchSize(0) {}

	  private:
		void reset()
		{
			vertices = 0;
			commands = 0;
			transparents = 0;
			instances = 0;
			batchSize = 0;
		}
		friend RenderStatistics;
	};

	class CommandPool
	{
	  public:
		unsigned int usedSize;
		unsigned int freeSize;
		unsigned int retrievals;

		CommandPool()
		    : usedSize(0), freeSize(0), retrievals(0) {}

	  private:
		void reset()
		{
			usedSize = 0;
			freeSize = 0;
			retrievals = 0;
		}
		friend RenderStatistics;
	};

	class Textures
	{
	  public:
		unsigned int count;
		unsigned long dataSize;

		Textures()
		    : count(0), dataSize(0) {}

	  private:
		void reset()
		{
			count = 0;
			dataSize = 0;
		}
		friend RenderStatistics;
	};

	/// Returns the aggregated command statistics for all types
	static inline const Commands &allCommands() { return allCommands_; }
	/// Returns the commnad statistics for the specified type
	static inline const Commands &commands(RenderCommand::CommandTypes::Enum type) { return typedCommands_[type]; }

	/// Returns statistics about the render command pools
	static inline const CommandPool &commandPool() { return commandPool_; }

	/// Returns aggregated texture statistics
	static inline const Textures &textures() { return textures_; }

	/// Returns the number of `DrawableNodes` culled because outside of the screen
	static inline unsigned int culled() { return culledNodes_[(index_ + 1) % 2]; }
#endif

	class Buffers
	{
	  public:
		unsigned int count;
		unsigned long size;
		unsigned long usedSpace;

		Buffers()
		    : count(0), size(0), usedSpace(0) {}

	  private:
		void reset()
		{
			count = 0;
			size = 0;
			usedSpace = 0;
		}
		friend RenderStatistics;
	};

	class CustomBuffers
	{
	  public:
		unsigned int count;
		unsigned long dataSize;

		CustomBuffers()
		    : count(0), dataSize(0) {}

	  private:
		void reset()
		{
			count = 0;
			dataSize = 0;
		}
		friend RenderStatistics;
	};

	class VaoPool
	{
	  public:
		unsigned int size;
		unsigned int capacity;
		unsigned int reuses;
		unsigned int bindings;

		VaoPool()
		    : size(0), capacity(0), reuses(0), bindings(0) {}

	  private:
		void reset()
		{
			size = 0;
			capacity = 0;
			reuses = 0;
			bindings = 0;
		}
		friend RenderStatistics;
	};

	/// Returns the buffer statistics for the specified type
	static inline const Buffers &buffers(RenderBuffersManager::BufferTypes::Enum type) { return typedBuffers_[type]; }

	/// Returns aggregated custom VBOs statistics
	static inline const CustomBuffers &customVBOs() { return customVbos_; }
	/// Returns aggregated custom IBOs statistics
	static inline const CustomBuffers &customIBOs() { return customIbos_; }

	/// Returns statistics about the VAO pool
	static inline const VaoPool &vaoPool() { return vaoPool_; }

  private:
	/// The string used to output OpenGL debug group information
	static nctl::String debugString_;

#ifdef WITH_SCENEGRAPH
	static Commands allCommands_;
	static Commands typedCommands_[RenderCommand::CommandTypes::COUNT];
	static CommandPool commandPool_;
	static Textures textures_;
	static unsigned int index_;
	static unsigned int culledNodes_[2];
#endif

	static Buffers typedBuffers_[RenderBuffersManager::BufferTypes::COUNT];
	static CustomBuffers customVbos_;
	static CustomBuffers customIbos_;
	static VaoPool vaoPool_;

	static void reset();

#ifdef WITH_SCENEGRAPH
	static void gatherStatistics(const RenderCommand &command);

	static inline void gatherCommandPoolStatistics(unsigned int usedSize, unsigned int freeSize)
	{
		commandPool_.usedSize = usedSize;
		commandPool_.freeSize = freeSize;
	}
	static inline void addCommandPoolRetrieval() { commandPool_.retrievals++; }

	static inline void addTexture(unsigned long datasize)
	{
		textures_.count++;
		textures_.dataSize += datasize;
	}
	static inline void removeTexture(unsigned long datasize)
	{
		textures_.count--;
		textures_.dataSize -= datasize;
	}

	static inline void addCulledNode() { culledNodes_[index_]++; }
#endif

	static void gatherStatistics(const RenderBuffersManager::ManagedBuffer &buffer);

	static inline void gatherVaoPoolStatistics(unsigned int poolSize, unsigned int poolCapacity)
	{
		vaoPool_.size = poolSize;
		vaoPool_.capacity = poolCapacity;
	}

	static inline void addCustomVbo(unsigned long datasize)
	{
		customVbos_.count++;
		customVbos_.dataSize += datasize;
	}
	static inline void removeCustomVbo(unsigned long datasize)
	{
		customVbos_.count--;
		customVbos_.dataSize -= datasize;
	}
	static inline void addCustomIbo(unsigned long datasize)
	{
		customIbos_.count++;
		customIbos_.dataSize += datasize;
	}
	static inline void removeCustomIbo(unsigned long datasize)
	{
		customIbos_.count--;
		customIbos_.dataSize -= datasize;
	}

	static inline void addVaoPoolReuse() { vaoPool_.reuses++; }
	static inline void addVaoPoolBinding() { vaoPool_.bindings++; }

	friend class ScreenViewport;
	friend class RenderQueue;
	friend class RenderBuffersManager;
	friend class Texture;
	friend class Geometry;
	friend class DrawableNode;
	friend class RenderVaoPool;
	friend class RenderCommandPool;
};

}

#endif
