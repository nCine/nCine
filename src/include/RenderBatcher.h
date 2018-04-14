#ifndef CLASS_NCINE_RENDERBATCHER
#define CLASS_NCINE_RENDERBATCHER

#include "RenderCommand.h"
#include "nctl/Array.h"
#include "nctl/UniquePtr.h"

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
	static const unsigned int UniformsBufferCapacity = 64 * 1024;

	nctl::Array<nctl::UniquePtr<RenderCommand> > freeCommandsPool_;
	nctl::Array<nctl::UniquePtr<RenderCommand> > usedCommandsPool_;

	/// Memory buffer to collect uniforms from all batched commands
	nctl::StaticArray<GLubyte, UniformsBufferCapacity> uniformsBuffer_;

	RenderCommand *collectCommands(nctl::Array<RenderCommand *>::ConstIterator start, nctl::Array<RenderCommand *>::ConstIterator end, nctl::Array<RenderCommand *>::ConstIterator &nextStart);
	RenderCommand *retrieveCommandFromPool(Material::ShaderProgramType shaderProgramType);
};

}

#endif
