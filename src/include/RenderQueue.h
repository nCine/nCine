#ifndef CLASS_NCINE_RENDERQUEUE
#define CLASS_NCINE_RENDERQUEUE

#include "RenderCommand.h"
#include <nctl/Array.h>

namespace ncine {

/// A class that sorts and issues the render commands collected by the scenegraph visit
class RenderQueue
{
  public:
	/// Constructor that sets the owning viewport
	RenderQueue();

	/// Returns true if the queue does not contain any render commands
	bool isEmpty() const;

	/// Adds a draw command to the queue
	void addCommand(RenderCommand *command);

	/// Sorts the queues, create batches and commits commands
	void sortAndCommit();
	/// Issues every render command in order
	void draw();

	/// Clears all the queues and resets the render batcher
	void clear();

  private:
	/// Array of opaque render command pointers
	nctl::Array<RenderCommand *> opaqueQueue_;
	/// Array of opaque batched render command pointers
	nctl::Array<RenderCommand *> opaqueBatchedQueue_;
	/// Array of transparent render command pointers
	nctl::Array<RenderCommand *> transparentQueue_;
	/// Array of transparent batched render command pointers
	nctl::Array<RenderCommand *> transparentBatchedQueue_;
};

}

#endif
