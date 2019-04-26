#ifndef CLASS_NCINE_RENDERBATCHER
#define CLASS_NCINE_RENDERBATCHER

#include "RenderCommand.h"
#include <nctl/Array.h>
#include <nctl/UniquePtr.h>

namespace ncine {

/// A class that batches render commands together
class RenderBatcher
{
  public:
	RenderBatcher();

	void collectInstances(const nctl::Array<RenderCommand *> &srcQueue, nctl::Array<RenderCommand *> &destQueue);
	void createBatches(const nctl::Array<RenderCommand *> &srcQueue, nctl::Array<RenderCommand *> &destQueue);
	void reset();

  private:
	static unsigned int MaxUniformBlockSize;

	struct ManagedBuffer
	{
		ManagedBuffer()
		    : size(0), freeSpace(0) {}

		unsigned int size;
		unsigned int freeSpace;
		nctl::UniquePtr<unsigned char[]> buffer;
	};

	nctl::Array<ManagedBuffer> buffers_;

	nctl::Array<nctl::UniquePtr<RenderCommand>> freeCommandsPool_;
	nctl::Array<nctl::UniquePtr<RenderCommand>> usedCommandsPool_;

	RenderCommand *collectCommands(nctl::Array<RenderCommand *>::ConstIterator start, nctl::Array<RenderCommand *>::ConstIterator end, nctl::Array<RenderCommand *>::ConstIterator &nextStart);
	RenderCommand *retrieveCommandFromPool(Material::ShaderProgramType shaderProgramType);

	unsigned char *acquireMemory(unsigned int bytes);
	void createBuffer(unsigned int size);
};

}

#endif
