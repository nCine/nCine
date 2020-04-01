#ifndef CLASS_NCINE_RENDERQUEUE
#define CLASS_NCINE_RENDERQUEUE

#include "RenderCommand.h"
#include "RenderBatcher.h"
#include <nctl/Array.h>

namespace ncine {

/// A class that sorts and issues the render commands collected by the scenegraph visit
class RenderQueue
{
  public:
	RenderQueue();

	/// Adds a draw command to the queue
	void addCommand(RenderCommand *command);
	/// Sorts the queues then issues every render command in order
	void draw();

  private:
	/// The string used to output OpenGL debug group information
	nctl::String debugGroupString_;

	/// Array of opaque render command pointers
	nctl::Array<RenderCommand *> opaqueQueue_;
	/// Array of opaque batched render command pointers
	nctl::Array<RenderCommand *> opaqueBatchedQueue_;
	/// Array of transparent render command pointers
	nctl::Array<RenderCommand *> transparentQueue_;
	/// Array of transparent batched render command pointers
	nctl::Array<RenderCommand *> transparentBatchedQueue_;

	RenderBatcher batcher_;
};

}

#endif
