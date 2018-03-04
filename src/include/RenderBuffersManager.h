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
			UNIFORM,

			COUNT
		};
	};

	struct Parameters
	{
		Parameters() : object(nullptr), size(0), offset(0), mapBase(nullptr) { }

		GLBufferObject *object;
		unsigned long size;
		unsigned long offset;
		GLubyte *mapBase;
	};

	RenderBuffersManager();

	/// Requests an amount of bytes from the specified buffer type
	const Parameters acquireMemory(BufferTypes::Enum type, unsigned long bytes);

  private:
	struct BufferSpecifications
	{
		BufferTypes::Enum type;
		GLenum target;
		GLenum mapFlags;
		GLenum usageFlags;
		unsigned long maxSize;
		GLuint alignment;
	};

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
