#ifndef CLASS_NCINE_RENDERBUFFERSMANAGER
#define CLASS_NCINE_RENDERBUFFERSMANAGER

#include "GLBufferObject.h"
#include "nctl/Array.h"
#include "nctl/UniquePtr.h"

namespace ncine {

/// The class handling the memory mapping in multiple OpenGL Buffer Objects
class RenderBuffersManager
{
  public:
	struct BufferTypes
	{
		enum Enum
		{
			ARRAY = 0,
			ELEMENT_ARRAY,
			UNIFORM,

			COUNT
		};
	};

	struct BufferSpecifications
	{
		BufferTypes::Enum type;
		GLenum target;
		GLenum mapFlags;
		GLenum usageFlags;
		unsigned long maxSize;
		GLuint alignment;
	};

	struct Parameters
	{
		Parameters() : object(nullptr), size(0), offset(0), mapBase(nullptr) { }

		GLBufferObject *object;
		unsigned long size;
		unsigned long offset;
		GLubyte *mapBase;
	};

	RenderBuffersManager(unsigned long vboMaxSize, unsigned long iboMaxSize);

	/// Returns the specifications for a buffer of the specified type
	inline const BufferSpecifications &specs(BufferTypes::Enum type) const { return specs_[type]; }
	/// Requests an amount of bytes from the specified buffer type
	inline Parameters acquireMemory(BufferTypes::Enum type, unsigned long bytes) { return acquireMemory(type, bytes, specs_[type].alignment); }
	/// Requests an amount of bytes from the specified buffer type with a custom alignment requirement
	Parameters acquireMemory(BufferTypes::Enum type, unsigned long bytes, unsigned int alignment);

  private:
	BufferSpecifications specs_[BufferTypes::COUNT];

	struct ManagedBuffer
	{
		ManagedBuffer()
			: size(0), freeSpace(0), mapBase(nullptr) { }

		BufferTypes::Enum type;
		nctl::UniquePtr<GLBufferObject> object;
		unsigned long size;
		unsigned long freeSpace;
		GLubyte *mapBase;
		nctl::UniquePtr<GLubyte []> hostBuffer;
	};

	nctl::Array<ManagedBuffer> buffers_;

	void flushUnmap();
	void remap();
	void createBuffer(const BufferSpecifications &specs);

	friend class RenderQueue;
	friend class RenderStatistics;
};

}

#endif
